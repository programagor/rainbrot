function pic=raindraw(dir, iters, dim, map, rgb)
%pic=zeros([dim(1),dim(2),3]);
if rgb==1
t=zeros(dim(2),dim(1),3);
end
for i=1:length(iters)-1
    fprintf('[%s] Iteration %d (%d-%d)\n',datetime,i,iters(i),iters(i+1)-1);
    fprintf('[%s]   Opening file...\n',datetime);
    f=fopen(sprintf('%s/%d-%d.rain',dir,iters(i),iters(i+1)-1));
    fprintf('[%s]   Reading file...\n',datetime);
    if rgb
        t(:,:,4-i)=fread(f,dim([2,1]),'uint64');
    else
        t=fread(f,dim([2,1]),'uint64');
    end
    fprintf('[%s]   Closing file...\n',datetime);
    fclose(f);
    fprintf('[%s]   Finding maximum...\n',datetime);
    if rgb
        m=max(max(t(:,:,4-i)));
    else
        m=max(max(t));
    end
    fprintf('[%s]   Applying transform...\n',datetime);
    %fun=@(x)sin(x*pi/2);
    fun=@(x)sqrt(-x.*(x-2));
    %fun=@(x)cos(asin(1-x));
    %t=t+t(end:-1:1,:); %mirroring
    for j=1:size(t,1) %save memory by considering 1 line at a time
        if rgb
            t(j,:,4-i)=fun(t(j,:,4-i)./m);
        else
            t(j,:)=fun(t(j,:)./m);
        end
    end
    if ~rgb
        fprintf('[%s]   Writing output image...\n',datetime);
        imwrite(uint8(t.*255),sprintf('%s/%d-%d.png',dir,iters(i),iters(i+1)-1));
        fprintf('[%s]   Freeing array...\n',datetime);
        clear('t');
    end
%    for c=1:3
%        pic(:,:,c)=pic(:,:,c)+t.*map(c,i);
%    end
    fprintf('[%s]   Done!\n',datetime);
end
if rgb
        fprintf('[%s]   Writing output image...\n',datetime);
        imwrite(uint8(t.*255),sprintf('%s/rgb8.png',dir));
        fprintf('[%s]   Freeing array...\n',datetime);
        clear('t');
    
end

fprintf('[%s] Batch finished!\n',datetime);

%for c=1:3
%    pic(:,:,c)=(pic(:,:,c)./max(max(pic(:,:,c))));
%end
    %figure;
    %imagesc(pic);

end