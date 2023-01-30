% In this script we read H5 data produced in C++:

clear all
close all
clc

saveFig = 1;

% Read data produced and saved by C++ code:
% home_dir = cd;
% cd ..
% cd output_files

fileName{1} = 'a.h5';
fileName{2} = 'b.h5';
fileName{3} = 'c.h5';
fileName{4} = 'point_index.h5';

for ii = 1:numel(fileName)
    data.(fileName{ii}(1:end-3)) = h5read(['../output_files/',fileName{ii}],['/dataset']);
end

x_data = data.a;
y_data = data.b;
i_index = data.point_index + 1;

figure('color','w');
box on
hold on
plot(x_data,y_data,'k.')
plot(x_data(i_index),y_data(i_index),'r.')
plot(x_data(i_index),y_data(i_index),'ro')
xlim([-1,1]*2)
ylim([0,1]*2)

% cd(home_dir);

% Save figure:
if saveFig
    folderName = '../output_files/';
    figureName = 'Step_1_find_results';
    
    % PDF figure:
    exportgraphics(gcf,[folderName,figureName,'.pdf'],'Resolution',600,'ContentType', 'vector') 

    % TIFF figure:
    exportgraphics(gcf,[folderName,figureName,'.tiff'],'Resolution',600) 
end