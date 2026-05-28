function angle = angle_interp1(x1d, y1d, x)	
    len = size(x1d,2);
	num = size(x,2);
	for i=1:num
	    xx = x(i);
		index1 = 0;
		if x1d(1) < x1d(len)
			if xx < x1d(1)
				xx = x1d(1);
			end
			if xx > x1d(len)
				xx = x1d(len);
			end			
			index1 = find(x1d>=xx,1);
		else
			if xx > x1d(1)
				xx = x1d(1);
			end
			if xx < x1d(len)
				xx = x1d(len);
			end
			index1 = find(x1d<=xx,1);
		end
		
		if xx == x1d(index1)
			angle(i) = y1d(index1);
			continue;
		end
		
		index0 = index1 -1;
		delt_angle = y1d(index1) - y1d(index0);
		while (delt_angle > pi)
			delt_angle = delt_angle-2*pi;
		end
		while (delt_angle < -pi)
			delt_angle = delt_angle + 2*pi;
		end;
		k = (xx-x1d(index0))/(x1d(index1)-x1d(index0));
		
		angle(i) = y1d(index0) + k*delt_angle;
		while (angle(i) < 0)
			angle(i) = angle(i) + 2*pi;
		end
		
		while (angle(i) > 2*pi)
			angle(i) = angle(i) - 2*pi;
		end
	end
	