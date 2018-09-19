function pic=raindraw(dir, iters, dim, map)
%pic=zeros([dim(1),dim(2),3]);
for i=1:length(iters)-1
    fprintf('[%s] Iteration %d (%d-%d)\n',datetime,i,iters(i),iters(i+1)-1);
    fprintf('[%s]   Opening file...\n',datetime);
    f=fopen(sprintf('%s/%d-%d',dir,iters(i),iters(i+1)-1));
    fprintf('[%s]   Reading file...\n',datetime);
    t=fread(f,dim,'uint64');
    fprintf('[%s]   Closing file...\n',datetime);
    fclose(f);
    fprintf('[%s]   Applying transform...\n',datetime);
    %fun=@(x)sin(x*pi/2);
    fun=@(x)sqrt(-x.*(x-2));
    %fun=@(x)cos(asin(1-x));
    %t=t+t(end:-1:1,:); %mirroring
    t=fun(t./max(max(t)));
%    for c=1:3
%        pic(:,:,c)=pic(:,:,c)+t.*map(c,i);
%    end
    fprintf('[%s]   Writing output image...\n',datetime);
    imwrite(uint16(t.*65535),sprintf('%s/%d-%d.png',dir,iters(i),iters(i+1)-1));
    fprintf('[%s]   Freeing array...\n',datetime);
    clear('t');
    fprintf('[%s]   Done!\n',datetime);
end
fprintf('[%s] Batch finished!\n',datetime);

%for c=1:3
%    pic(:,:,c)=(pic(:,:,c)./max(max(pic(:,:,c))));
%end
    %figure;
    %imagesc(pic);

end