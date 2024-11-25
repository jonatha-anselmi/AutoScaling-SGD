function cost_function_plot()

	close all
	x=load('cost_function_data.txt');
	
	wid=600; hei=600; figure('Renderer', 'painters', 'Position', [0 500 wid hei]);

	a = get(gca,'XTickLabel'); set(gca,'fontsize',14), set(gcf,'color','w'), 
	plot(x(:,1),x(:,2), 'k*-'), grid on;

	xlabel('\theta', 'FontSize', 16),
	ylabel('Cost function', 'FontSize', 14);


