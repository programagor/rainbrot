function raindraw(dir, iters, dim, map)
pic=zeros([dim(1),dim(2),3]);
for i=1:length(iters)-1
    f=fopen(sprintf('%s/%d-%d',dir,iters(i),iters(i+1)-1));
    t=fread(f,dim,'uint64');
    for c=1:3
        pic(:,:,c)=pic(:,:,c)+t.*map(c,i);
    end
end

for c=1:3
    pic(:,:,c)=pic(:,:,c)./max(max(pic(:,:,c)));
end
    %res=uint16(pic.*65535);
    figure;
    imagesc(pic);

end