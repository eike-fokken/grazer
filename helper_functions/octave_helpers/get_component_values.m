function m = get_component_values(filename,variablename)
  pkg load io;
    
  filecontents=csv2cell(filename,",");
  newcell=strrep(filecontents(1,:)," ","");
  a = ismember(newcell,variablename);
  m = cell2mat(filecontents(2:end,a));
end
