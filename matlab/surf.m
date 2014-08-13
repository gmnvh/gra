path = 'D:\Gustavo\Pessoal\Mestrado\FEI\Tese\photos\hand_pose_ir_ref\ir_led_1\';
filename = '0_04.jpg';

path_t = 'D:\Gustavo\Pessoal\Mestrado\FEI\Tese\photos\hand_pose_ir_ref\ir_led_1\';
filename_t = '9_01.jpg';

I = imread(strcat(path,filename));
R = imrotate(I, -90);

pointa = detectSURFFeatures(R);
[f1, vpts1] = extractFeatures(R, pointa);

%I = rgb2gray(imread(strcat(path_t,filename_t)));
%I = imrotate(I, -90);
I = imread(strcat(path_t,filename_t));
I = imrotate(I, -90);

pointb = detectSURFFeatures(I);
[f2, vpts2] = extractFeatures(I, pointb);

indexPairs = matchFeatures(f1, f2, 'Prenormalized', true) ;
matchedPoints1 = vpts1(indexPairs(:, 1));
matchedPoints2 = vpts2(indexPairs(:, 2));

xy1 = min(matchedPoints2.Location)
xy2 = max(matchedPoints2.Location)
roi = [xy1 xy2-xy1]
z = insertShape(I, 'Rectangle', roi, 'Color', 'green');
imshow(z);
%figure; showMatchedFeatures(R,I,matchedPoints1,matchedPoints2);
%legend('matched points 1','matched points 2');