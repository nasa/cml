clc; clear; close all;

precision = 16;
% % digits(precision);
% format longG;

% Position-Velocity (PV) state samples in the inertial frame (each row is a 6D state vector)
inertial_pv_states = [ % (10 x 6)
    7000.0,   0.0,    0.0,   0.00,  7.50,  0.00;
    7100.0, 100.0,  -50.0,   0.10,  7.60, -0.05;
    6900.0,-120.0,   80.0,  -0.20,  7.40,  0.10;
    7050.0,  50.0,  -30.0,   0.05,  7.55, -0.02;
    6950.0, -60.0,   60.0,  -0.10,  7.45,  0.08;
    7150.0, 130.0,  -70.0,   0.15,  7.65, -0.06;
    6850.0,-140.0,   90.0,  -0.25,  7.35,  0.12;
    7000.0,  20.0,  -10.0,   0.00,  7.50, -0.01;
    6950.0, -80.0,   40.0,  -0.12,  7.48,  0.05;
    7050.0,  90.0,  -60.0,   0.08,  7.52, -0.04
];

% PV Covariance Matrix for inertial states
inertial_pv_cov = cov(inertial_pv_states); % (6 x 6)
disp(num2str(reshape(inertial_pv_cov', 1, []), precision));
disp(det(inertial_pv_cov));
disp(trace(inertial_pv_cov));

% Transform PV state samples to new reference frame
yaw = 321;
pitch = 243;
roll = 56;
T_1 = [1 0 0;
       0 cosd(roll) sind(roll);
       0 -sind(roll) cosd(roll)];
T_2 = [cosd(pitch) 0 -sind(pitch);
       0 1 0;
       sind(pitch) 0 cosd(pitch)];
T_3 = [cosd(yaw) sind(yaw) 0;
       -sind(yaw) cosd(yaw) 0;
       0 0 1];
T_inertial_to_reference = T_1 * T_2 * T_3;
disp(num2str(reshape(T_inertial_to_reference', 1, []), precision));

% T_inertial_to_reference = angle2dcm(deg2rad(yaw), deg2rad(pitch), deg2rad(roll), 'ZYX');
% disp(num2str(reshape(T_inertial_to_reference', 1, []), disp_precision));

ref_frame_pv_states = zeros(size(inertial_pv_states));
for i = 1:size(inertial_pv_states, 1)
    r = inertial_pv_states(i, 1:3)';
    v = inertial_pv_states(i, 4:6)';

    r_rot = T_inertial_to_reference * r;
    v_rot = T_inertial_to_reference * v;

    ref_frame_pv_states(i, :) = [r_rot', v_rot'];
end

% PV Covariance Matrix for new reference frame states
ref_frame_pv_cov = cov(ref_frame_pv_states);
disp(num2str(reshape(ref_frame_pv_cov', 1, []), precision));
disp(det(ref_frame_pv_cov));
disp(trace(ref_frame_pv_cov));

% Transformation matrix back to inertial frame
T_reference_to_inertial = T_inertial_to_reference';
disp(num2str(reshape(T_reference_to_inertial', 1, []), precision));
