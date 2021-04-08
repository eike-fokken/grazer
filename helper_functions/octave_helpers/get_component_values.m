function m = get_component_values(directory,subtype,id,variablename)
  pkg load io;
    
  filename = strcat(subtype,"_",id);
  full_filename= fullfile(directory,filename);

  filecontents=csv2cell(full_filename,",");
  newcell=strrep(filecontents(1,:)," ","");
  [a,voltage_index] = ismember(newcell,variablename);
  m = cell2mat(filecontents(2:end,a));
end
