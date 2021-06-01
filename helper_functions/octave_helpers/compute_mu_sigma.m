function [mu,sigma,dirlist] = compute_mu_sigma(working_directory,output_directory_trunk,subtype,variablename)

  mu=1;
  sigma=3;

  dirlist=get_all_output_directories(working_directory,output_directory_trunk);


  %% get number of runs:
  number_of_runs= length(dirlist);


  first_comp_dir = dir(fullfile(dirlist(1).folder,dirlist(1).name,subtype));
  number_of_components = length(first_comp_dir)-2; % ignore . and .. !

  number_of_timesteps = length(get_component_values(fullfile(first_comp_dir(3).folder,first_comp_dir(3).name),variablename))

  m = cell(1,number_of_components);
  

  for idir= 1:1## length(dirlist)
    component_dir = fullfile(dirlist(1).folder,dirlist(idir).name,subtype);
    component_files = dir(component_dir);

    iraw=1;
    while(iraw~=length(component_files)+1)
      first_character = substr(component_files(iraw).name,1,1);
      if(first_character == ".")
        component_files(iraw)=[];
      else
        iraw = iraw+1;
      end
    end


    for icompdir = 1:length(component_files)
      current_file = fullfile(component_files(icompdir).folder,component_files(icompdir).name);
      values = get_component_values(current_file,variablename);
      m{icompdir} = values;
    end
  end


end
