function add_voltage(id,directory)
  pkg load io;
    
  filename = strcat("Power_",id)
  full_filename= fullfile(directory,filename)

  filecontents=csv2cell(full_filename,",");
  filecontents(1,:)
  newcell=strrep(filecontents(1,:)," ","")
  [a,voltage_index] = ismember(newcell,"V")
  m = cell2mat(filecontents(2:end,a))
  class(m)

end
