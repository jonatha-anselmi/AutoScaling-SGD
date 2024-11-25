function kw_plot()

	close all
	figure('Renderer', 'painters', 'Position', [0 600 1200 600]); % [. . wid hei]

subplot(1,2,1)
	x50 =load('cost_function_data_N50.txt');
	x100=load('cost_function_data_N50_L015.txt');

	a = get(gca,'XTickLabel'); set(gca,'fontsize',14), set(gcf,'color','w'), 
	plot(x50(:,1),x50(:,2), 'k*-'), grid on;
	hold on, plot(x100(:,1),x100(:,2), 'b*-');
	xline(6,'k--',{'\theta^*'},   'LabelOrientation','aligned','FontSize',14, 'LabelVerticalAlignment','mid', 'LabelHorizontalAlignment','right', 'LineWidth', 2);
	xline(5,'b--',{'   \theta^*'},   'LabelOrientation','aligned','FontSize',14, 'LabelVerticalAlignment','mid', 'LabelHorizontalAlignment','left', 'LineWidth', 2);

	xline(15,'r-','LineWidth', 2);

	xlabel('\theta', 'FontSize', 16),
	ylabel('Cost function - f(\theta)', 'FontSize', 14);
	axis([0 30 26 44])

legend('\lambda=0.3', '\lambda=0.15', 'Location','southeast', 'FontSize', 14);

xticks([0 5 6 10 15 20 25 30 35])
xticklabels({'0','5','6','10','15','20','25','30','35'})

subplot(1,2,2)

	y_1 =load('as_kw_res_theta1N50.txt');
	y_10=load('as_kw_res_theta10N50.txt');
	y_1_15=load('as_kw_res_theta1_L015.txt');
	y_10_15=load('as_kw_res_theta10_L015.txt');
	
	a = get(gca,'XTickLabel'); set(gca,'fontsize',14), set(gcf,'color','w'), 
	plot(y_1(:,1), 'k*-'), grid on;
	hold on, plot(y_1_15(:,1), 'b*-');
	hold on, plot(y_10(:,1), 'k*--');
	hold on, plot(y_10_15(:,1), 'b*--');

	xlabel('n (# iterations on \theta_n by Algorithm 2)', 'FontSize', 14),
	ylabel('\theta_n', 'FontSize', 14);

%	axis([0 25.5 1 13])

	legend('\lambda=0.3, \theta_0=1', '\lambda=0.15, \theta_0=1',...
	 '\lambda=0.3, \theta_0=10', '\lambda=0.15, \theta_0=10', ...
	  'Location','southeast', 'FontSize', 14);

