--InitCommandGroup(); --start group
--MoveTo(true, "frank", 20.0,6.0,20.0, 4.0, 0.1, 0.1); -- first parameter true=serial command, else parallel
--FollowObject(true, "frank");
--MoveTo(true, "frank", 15.0,6.0,15.0, 10.0, 0.1, 0.1);
--SubmitCommandGroup(); --submit group


--InitCommandGroup();
--Wait(14.1);
--RotateObject(true, "frank", 1.0,1.0,1.0,2.0);
--x,y,z = GetObjectPosition("pod");
--print(x);
--print(y);
--print(z);
--MoveTo(true, "frank", x, y, z, 6.0, 0.1, 0.1); 
--SubmitCommandGroup();