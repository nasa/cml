% This small script is simply a double check of the outputs of RUN_GrowHoldDecay
% Here we perform the twist sway transformation for a single data point as a way
% of verifying the result we see in the unit sim
% Inputs: sway_normal, sway_parallel, twist
% Output: dp_enu[3] (total displacement of body relative to base expressed
%                    in the ENU frame. See docs for details)

% Input data from t=5.0 sec in RUN_GrowHoldDecay
sway_normal   = 0.079796913;    % meters
sway_parallel = 0.0268172097;   % meters
twist         = 0.000477898547; % rad
wind_dir      = 0.0;            % rad  (due north) not used
rocket_height = 30.0;           % meters

% See documentation for visual explanation of these variables
% e: component of sway in east dir
% n: component of sway in north dir
% u: component of sway in up dir (need to solve for)
% h: magnitude of horizontal sway
%   e*e + n*n = h*h
%   h*h + u*u = H*H
%   enu_displacement = [e, n, u-H]
H = rocket_height;
e = -sway_normal;  % Since wind_dir = 0
n = sway_parallel; % Since wind_dir = 0
h = sqrt(e^2 + n^2);
u = sqrt(H^2 - h^2);
enu_displacement = [e; n; u-H];

% Assert the values are what we expect
tol = 1e-8 ;
assert(enu_displacement(1), -7.97969130000000e-02, tol);
assert(enu_displacement(2), 2.68172097000000e-02, tol);
assert(enu_displacement(3), -1.18112066846976e-04, tol);

ref_H = [0; 0; H]; % Reference location at height H
ref_H_unit = ref_H/norm(ref_H) ;% unit vec
ref_H_swayed = ref_H + enu_displacement;
ref_H_swayed_unit = ref_H_swayed/ norm(ref_H_swayed );

%% Compute the transformation from ENU to SWAY:
s =  cross( ref_H_unit , ref_H_swayed_unit );
phi = asin(norm(s))  % angle between vectors
s_unit = s/norm(s);

% Use Rodrigues' Formula as independent verification of the model's computation
%  see https://en.wikipedia.org/wiki/Rodrigues'_rotation_formula#Matrix_notation
I = [1 0 0; 0 1 0; 0 0 1];
K = [0 -s_unit(3) s_unit(2); s_unit(3) 0 -s_unit(1); -s_unit(2) s_unit(1) 0];
R = I + sin(phi) * K + (1- cos(phi))* K*K;
T_sway_to_enu = R;
T_enu_to_sway = T_sway_to_enu ^-1;

T_sway_to_twistsway = [ cos(twist), sin(twist), 0;
                       -sin(twist), cos(twist), 0;
                            0,          0,      1];
T_enu_to_twistsway = T_sway_to_twistsway * T_enu_to_sway

assert(T_enu_to_twistsway(1,1), 0.999996349, tol);
assert(T_enu_to_twistsway(1,2), 0.000479087, tol);
assert(T_enu_to_twistsway(1,3), 0.002659469, tol);
assert(T_enu_to_twistsway(2,1), -0.000476707985, tol);
assert(T_enu_to_twistsway(2,2), 0.999999486, tol);
assert(T_enu_to_twistsway(2,3), -0.000895178048, tol);
assert(T_enu_to_twistsway(3,1), -0.0026598971, tol);
assert(T_enu_to_twistsway(3,2), 0.000893906989, tol);
assert(T_enu_to_twistsway(3,3), 0.999996063, tol);

fprintf('RUN_GrowHoldDecay @ t=5.0 sec\n')
fprintf('  INPUTS:\n')
fprintf('    sway_normal:   %f \n', sway_normal)
fprintf('    sway_parallel: %f \n', sway_parallel)
fprintf('    twist:         %f \n', twist)
fprintf('    rocket_height: %f \n', rocket_height)
fprintf('  OUTPUTS:\n')
fprintf('    ENU displacement: %f, %f, %f (m) \n', enu_displacement(1),enu_displacement(2),enu_displacement(3))
fprintf('    T_enu_to_ts:      %-15.9f  %-15.9f  %-15.9f \n', T_enu_to_twistsway(1,1),T_enu_to_twistsway(1,2),T_enu_to_twistsway(1,3))
fprintf('                      %-15.9f  %-15.9f  %-15.9f \n', T_enu_to_twistsway(2,1),T_enu_to_twistsway(2,2),T_enu_to_twistsway(2,3))
fprintf('                      %-15.9f  %-15.9f  %-15.9f \n', T_enu_to_twistsway(3,1),T_enu_to_twistsway(3,2),T_enu_to_twistsway(3,3))

fprintf('If you got this far, the test passed because asserts will fail in line.\n')
fprintf('Read the comments in this script to understand whats going on.')
