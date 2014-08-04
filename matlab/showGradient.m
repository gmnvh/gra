path = 'd:\Gustavo\Pessoal\Mestrado\FEI\Tese\photos\hand_pose_ir_ref\night\focus\gustavo\blackshirt\';
filename = '02.jpg';

save_path = 'D:\Gustavo\Pessoal\Mestrado\FEI\Tese\rep\latex\image\';

I = rgb2gray(imread(strcat(path,filename)));
[PGmag, PGdir] = imgradient(I, 'prewitt');
[PGx, PGy] = imgradientxy(I, 'prewitt');

[SGmag, SGdir] = imgradient(I, 'sobel');
[SGx, SGy] = imgradientxy(I, 'sobel');

imwrite(I, strcat(save_path, 'gradiente_original.jpg'));
imwrite(mat2gray(PGmag), strcat(save_path, 'gradiente_prewitt_mag.jpg'));
imwrite(mat2gray(PGx), strcat(save_path, 'gradiente_prewitt_gx.jpg'));
imwrite(mat2gray(PGy), strcat(save_path, 'gradiente_prewitt_gy.jpg'));

imwrite(mat2gray(SGmag), strcat(save_path, 'gradiente_sobel_mag.jpg'));
imwrite(mat2gray(SGx), strcat(save_path, 'gradiente_sobel_gx.jpg'));
imwrite(mat2gray(SGy), strcat(save_path, 'gradiente_sobel_gy.jpg'));

figure;
subplot(3,3,2), subimage(I), axis off, title('Imagem original'),
subplot(3,3,4), imshow(PGmag, []), axis off, title('Prewitt Modulo'),
subplot(3,3,5), imshow(PGx, []), axis off, title('Prewitt Gx'),
subplot(3,3,6), imshow(PGy, []), axis off, title('Prewitt Gy');
subplot(3,3,7), imshow(SGmag, []), axis off, title('Sobel Modulo'),
subplot(3,3,8), imshow(SGx, []), axis off, title('Sobel Gx'),
subplot(3,3,9), imshow(SGy, []), axis off, title('Sobel Gy');