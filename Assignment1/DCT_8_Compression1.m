
close all; clear all; clc;
N=512; % signal length
L=8; % transform length
M=4; % select the number of DCT coefficients 

%% Load the test signal

load('ecgsig.mat')
% x=ecgsig(1:N,1);
%% dct transform of signal
H=dctmtx(L);

for i=1:N/L
    y(:,i)=H*x(L*(i-1)+1:L*i);
end
%% Select first M coefficients for each DCT transformed singal

y_transmit=y(1:M,:);


%% To reconstruct the orignal signal
y_receive=zeros(L,N/L);
y_receive(1:M,:)=y_transmit;
x_rec=[];
invH=H^(-1);
for i=1:N/L
x_rec=[x_rec;invH*y_receive(:,i)];
end


plot(x);
hold on;
plot((x_rec), 'r.');
legend('Original', 'Recovered','Location','best');