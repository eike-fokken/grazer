function m = get_power_values(id,directory,variablename)
  pkg load io;
    
  filename = strcat("Power_",id);
  full_filename= fullfile(directory,filename);

  filecontents=csv2cell(full_filename,",");
  newcell=strrep(filecontents(1,:)," ","");
  [a,voltage_index] = ismember(newcell,variablename);
  m = cell2mat(filecontents(2:end,a));
end
