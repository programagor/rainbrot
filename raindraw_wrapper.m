dim=[128 128];
dir=sprintf('mandelbrot_%ux%u(-2.000000+-2.000000i_2.000000+2.000000i)-64.000000',dim(1),dim(2));
iter=[100,10000];
%[r,g,b]=sensitivity(0:(1/(length(iter)-2)):1);
r=1;
g=1;
b=1;
raindraw(dir,iter,dim,[r;g;b]);