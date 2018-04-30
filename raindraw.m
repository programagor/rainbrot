%mat_data.m;
image=zeros(400,400,3);
image(:,:,1)=(x4+x5);
image(:,:,1)=image(:,:,1)./max(max(image(:,:,1)));
image(:,:,1)=image(:,:,1).^0.25;
image(:,:,2)=(x2+x3);
image(:,:,2)=image(:,:,2)./max(max(image(:,:,2)));
image(:,:,2)=image(:,:,2).^0.65;
image(:,:,3)=x1;    
image(:,:,3)=image(:,:,3)./max(max(image(:,:,3)));
image(:,:,3)=image(:,:,3).^0.85;

figure, imshow(image);