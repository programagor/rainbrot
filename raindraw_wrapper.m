dim=[65536 24576];
%dim=[400 400]
dir=sprintf('ship_65536x24576_(-1.9)+(-0.1i)_(-1.5)+(0.05i)-64',dim(1),dim(2));
%dir='mandelbrot_400x400_(-2)+(-2i)_(2)+(2i)-40'
iter=[10 50 250 1250];
%[r,g,b]=sensitivity(0:(1/(length(iter)-2)):1);
r=1;
g=1;
b=1;
raindraw(dir,iter,dim,[r;g;b],1);