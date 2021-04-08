function [mu,sigma] = compute_mu_sigma(working_directory,output_directory_trunk,subtype,ids,variablename)

  mu=1;
  sigma=3;
  m=[];

  output_directories=dir(working_directory);
  index=1;
  while(index~=length(output_directories)+1)
    if(strfind(output_directories(index).name,output_directory_trunk))
      index=index+1;
    else
      output_directories(index)=[];
    end
  end
  output_directories.name

  %% works till here!

  %% for id
  %% for iids=1:length(ids)
  %%   m = [m,get_component_values(directory,subtype,ids(iids),variablename)
  %% end


  




end
