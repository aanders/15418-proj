% Make pretty plots of data
%
% 15-418 project
% Team Veggie Tales

%
% Load result sets
%
% results = csvread('results-ghc.csv');
results = csvread('results-laptop.csv');

tvt = csvread('treevtree.csv');

% Get data columns
SCTrial  = results(:,1);
SCOps    = results(:,2);
SCInsert = results(:,3);
SCLookup = results(:,4);
SCDelete = results(:,5);
RBTrial  = results(:,6);
RBOps    = results(:,7);
RBInsert = results(:,8);
RBLookup = results(:,9);
RBDelete = results(:,10);
ATTrial  = tvt(:,1);
ATOps    = tvt(:,2);
ATInsert = tvt(:,3);
ATLookup = tvt(:,4);
ATDelete = tvt(:,5);

%
% Get row indices for parameters
%

% Array versions
v4 = logical([ones(192,1); zeros(192*2,1)]);
v8 = logical([zeros(192,1); ones(192,1); zeros(192,1)]);
v9 = logical([zeros(192*2,1); ones(192,1)]);
% Trace runners
disk = logical(repmat([ones(96,1); zeros(96,1)],3,1));
mem  = logical(repmat([zeros(96,1); ones(96,1)],3,1));
% Lengths
l16k   = logical(repmat([ones(16,1); zeros(16*5,1)],6,1));
l64k   = logical(repmat([zeros(16,1); ones(16,1); zeros(16*4,1)],6,1));
l128k  = logical(repmat([zeros(16*2,1); ones(16,1); zeros(16*3,1)],6,1));
l512k  = logical(repmat([zeros(16*3,1); ones(16,1); zeros(16*2,1)],6,1));
l1024k = logical(repmat([zeros(16*4,1); ones(16,1); zeros(16,1)],6,1));
l2048k = logical(repmat([zeros(16*5,1); ones(16,1)],6,1));
% Biases
buniform = logical(repmat([ones(4,1); zeros(4*3,1)],36,1));
blookup  = logical(repmat([zeros(4,1); ones(4,1); zeros(4*2,1)],36,1));
binsert  = logical(repmat([zeros(4*2,1); ones(4,1); zeros(4,1)],36,1));
bburst   = logical(repmat([zeros(4*3,1); ones(4,1)],36,1));
% Delays
d100us = logical(repmat([1;0;0;0],144,1));
d500us = logical(repmat([0;1;0;0],144,1));
d1ms   = logical(repmat([0;0;1;0],144,1));
d5ms   = logical(repmat([0;0;0;1],144,1));

% Values for x axis
arrays = [1,2,3];
runners = [1,2];
lengths = [16,64,128,512,1024,2048];
biases = [1,2,3,4];
delays = [100,500,1000,5000];

%
% PLOTS WOOO
% 

%% [V9 Mem 1ms] Op time vs Trace Len for Trace Bias
figure;
plot( ...
    lengths, SCOps(v9&mem&buniform&d1ms), ...
    lengths, RBOps(v9&mem&buniform&d1ms), ...
    lengths, ATOps(v4&mem&buniform&d1ms)  ...
);
title('Total op time (uniform trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Total time in data structure ops (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCOps(v9&mem&blookup&d1ms), ...
    lengths, RBOps(v9&mem&blookup&d1ms), ...
    lengths, ATOps(v4&mem&blookup&d1ms)  ...
);
title('Total op time (lookup trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Total time in data structure ops (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCOps(v9&mem&binsert&d1ms), ...
    lengths, RBOps(v9&mem&binsert&d1ms), ...
    lengths, ATOps(v4&mem&binsert&d1ms)  ...
);
title('Total op time (insert trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Total time in data structure ops (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCOps(v9&mem&bburst&d1ms), ...
    lengths, RBOps(v9&mem&bburst&d1ms), ...
    lengths, ATOps(v4&mem&bburst&d1ms)  ...
);
title('Total op time (flip trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Total time in data structure ops (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;

%% [V9 Mem 1ms] INSERT time vs Trace Len for Trace Bias
figure;
plot( ...
    lengths, SCInsert(v9&mem&buniform&d1ms), ...
    lengths, RBInsert(v9&mem&buniform&d1ms), ...
    lengths, ATInsert(v4&mem&buniform&d1ms)  ...
);
title('Avg insert time (uniform trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg insert time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCInsert(v9&mem&blookup&d1ms), ...
    lengths, RBInsert(v9&mem&blookup&d1ms), ...
    lengths, ATInsert(v4&mem&blookup&d1ms)  ...
);
title('Avg insert time (lookup trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg insert time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCInsert(v9&mem&binsert&d1ms), ...
    lengths, RBInsert(v9&mem&binsert&d1ms), ...
    lengths, ATInsert(v4&mem&binsert&d1ms)  ...
);
title('Avg insert time (insert trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg insert time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCInsert(v9&mem&bburst&d1ms), ...
    lengths, RBInsert(v9&mem&bburst&d1ms), ...
    lengths, ATInsert(v4&mem&bburst&d1ms)  ...
);
title('Avg insert time (flip trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg insert time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;

%% [V9 Mem 1ms] LOOKUP time vs Trace Len for Trace Bias
figure;
plot( ...
    lengths, SCLookup(v9&mem&buniform&d1ms), ...
    lengths, RBLookup(v9&mem&buniform&d1ms), ...
    lengths, ATLookup(v4&mem&buniform&d1ms)  ...
);
title('Avg lookup time (uniform trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg lookup time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCLookup(v9&mem&blookup&d1ms), ...
    lengths, RBLookup(v9&mem&blookup&d1ms), ...
    lengths, ATLookup(v4&mem&blookup&d1ms)  ...
);
title('Avg lookup time (lookup trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg lookup time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCLookup(v9&mem&binsert&d1ms), ...
    lengths, RBLookup(v9&mem&binsert&d1ms), ...
    lengths, ATLookup(v4&mem&binsert&d1ms)  ...
);
title('Avg lookup time (insert trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg lookup time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCLookup(v9&mem&bburst&d1ms), ...
    lengths, RBLookup(v9&mem&bburst&d1ms), ...
    lengths, ATLookup(v4&mem&bburst&d1ms)  ...
);
title('Avg lookup time (flip trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg lookup time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;

%% [V9 Mem 1ms] DELETE time vs Trace Len for Trace Bias
figure;
plot( ...
    lengths, SCDelete(v9&mem&buniform&d1ms), ...
    lengths, RBDelete(v9&mem&buniform&d1ms), ...
    lengths, ATDelete(v4&mem&buniform&d1ms)  ...
);
title('Avg delete time (uniform trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg delete time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCDelete(v9&mem&blookup&d1ms), ...
    lengths, RBDelete(v9&mem&blookup&d1ms), ...
    lengths, ATDelete(v4&mem&blookup&d1ms)  ...
);
title('Avg delete time (lookup trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg delete time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCDelete(v9&mem&binsert&d1ms), ...
    lengths, RBDelete(v9&mem&binsert&d1ms), ...
    lengths, ATDelete(v4&mem&binsert&d1ms)  ...
);
title('Avg delete time (insert trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg delete time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
figure;
plot( ...
    lengths, SCDelete(v9&mem&bburst&d1ms), ...
    lengths, RBDelete(v9&mem&bburst&d1ms), ...
    lengths, ATDelete(v4&mem&bburst&d1ms)  ...
);
title('Avg delete time (flip trace bias)');
legend('SortedCollection', 'Sequential Tree', 'Async Tree');
ylabel('Avg delete time (ms)');
xlabel('Trace length (10^3 instructions)');
grid on;
