function kw_plot_nonstationary()

	close all
	figure('Renderer', 'painters', 'Position', [0 600 1200 400]); % [. . wid hei]

subplot(1,2,1)
theta=1;
	y_1=load(sprintf('as_kw_res_theta%d_L03_100.txt',theta));
	y_1_15=load(sprintf('as_kw_res_theta%d_L015_100.txt',theta));
theta=10;
	y_10=load(sprintf('as_kw_res_theta%d_L03_100.txt',theta));
	y_10_15=load(sprintf('as_kw_res_theta%d_L015_100.txt',theta));
	
	a = get(gca,'XTickLabel'); set(gca,'fontsize',14), set(gcf,'color','w'), 
	plot(y_1(:,1), 'k-','LineWidth',2), grid on;
	hold on, plot(y_1_15(:,1), 'b-','LineWidth',2);
	hold on, plot(y_10(:,1), 'k--','LineWidth',2);
	hold on, plot(y_10_15(:,1), 'b--','LineWidth',2);

	xlabel('n (# iterations on \theta_n by Algorithm 2)', 'FontSize', 14),
	ylabel('\theta_n', 'FontSize', 14);

	title('\theta_n updates every 100 state changes')
	
	axis([0 min(size(y_1(:,1),1),size(y_1_15(:,1),1)) 0 21]);

	legend('\lambda=0.3, \theta_0=1', '\lambda=0.15, \theta_0=1',...
	 '\lambda=0.3, \theta_0=10', '\lambda=0.15, \theta_0=10', ...
	  'Location','northwest', 'FontSize', 12);

subplot(1,2,2)

%	y_1 =load('as_kw_res_theta1N50.txt');
%	y_10=load('as_kw_res_theta10N50.txt');
%	y_1_15=load('as_kw_res_theta1_L015.txt');
%	y_10_15=load('as_kw_res_theta10_L015.txt');

theta=1;
	y_1=load(sprintf('as_kw_res_theta%d_L03_1000.txt',theta));
	y_1_15=load(sprintf('as_kw_res_theta%d_L015_1000.txt',theta));
theta=10;
	y_10=load(sprintf('as_kw_res_theta%d_L03_1000.txt',theta));
	y_10_15=load(sprintf('as_kw_res_theta%d_L015_1000.txt',theta));
	
	a = get(gca,'XTickLabel'); set(gca,'fontsize',14), set(gcf,'color','w'), 
	plot(y_1(:,1), 'k-','LineWidth',2), grid on;
	hold on, plot(y_1_15(:,1), 'b-','LineWidth',2);
	hold on, plot(y_10(:,1), 'k--','LineWidth',2);
	hold on, plot(y_10_15(:,1), 'b--','LineWidth',2);

	xlabel('n (# iterations on \theta_n by Algorithm 2)', 'FontSize', 14),
	ylabel('\theta_n', 'FontSize', 14);
	
	title('\theta_n updates every 1000 state changes')

	axis([0 min(size(y_1(:,1),1),size(y_1_15(:,1),1)) 0.5 10.5]);

	legend('\lambda=0.3, \theta_0=1', '\lambda=0.15, \theta_0=1',...
	 '\lambda=0.3, \theta_0=10', '\lambda=0.15, \theta_0=10', ...
	  'Location','northeast', 'FontSize', 12);

