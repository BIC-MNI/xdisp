function out = xdisp(image, var_name, xtra_cmd_opts)
% Usage: xdisp(image [, xdisp_window_title [, command_line_options]])
%
%
% e.g.  xdisp(im1,'Cool_Image','-spectral -color_bar');
%
% Displays matrix im1 using xdisp and creates a window named 'Cool_Image'
% in which the color table is spectral and a color bar is shown at the
% right.
%

% xdisp.m 
% v4.2,  Oct 27, 1997
%
% Copyright (c) B Pike, 1997

% check for number of arguments
if  nargin < 1
  error('XDISP requires at least one input argument.');
elseif  nargin == 1     
  fname = 'matlab_image'; % no variable name given: used default
  xtra_cmd_opts = ' ';    % no extra options selected: leave empty
elseif  nargin == 2 
  fname = var_name;       % use provided variable name
  xtra_cmd_opts = ' ';    % no extra options selected: leave empty
else  
  fname = var_name;       % use provided variable name
end

% replace spaces with underscores in fname
for i=1:length(fname)
  if (fname(i)==' ') 
    fname(i) = '_';
  end
end

% set any standard options here
cmd_opts = ' -TrueColor ';

% check Matlab version to see if n-D arrays are supported
[ml_version ml_date] = version;

if (str2num(ml_version(1)) >= 5) % n-D stuff ok
  % get the image size (handle n-D volumes as 3D)
  [rows cols slices] = size(image);
  if (length(size(image)) > 3) 		% notify use if greater than 3-D
    disp(['NOTE: Your ', num2str(length(size(image))), '-D data ', ...
	  'will be interpreted as a 3-D image volume by xdisp.']);
  end;
else                             % n-D stuff not ok
  [rows cols] = size(image);
end;

% matlab stores data column wise so fix this
if (str2num(ml_version(1)) >= 5)  % n-D allowed stuff
  if (rows~=cols)  % can't transpose non-square images in place
    tmp_image=zeros(cols,rows,slices);
    for i=1:slices 
      % Note: have to use eval to get around Matlab 4 complaints
      % about illegal scripting despite the fact that this code is
      % selected only if running Matlab 5.
      eval('tmp_image(:,:,i) = transpose(image(:,:,i));');
    end;
    clear image;
    image = tmp_image;
  else 		   % transpose in place
    for i=1:slices
      % Note: have to use eval to get around Matlab 4 complaints
      % about illegal scripting despite the fact that this code is
      % selected only if running Matlab 5.
      eval('image(:,:,i) = transpose(image(:,:,i));');
    end;
  end;
else % n-D not allowed stuff
  image = image';
end;

% now get correct dimensions
if (str2num(ml_version(1)) >= 5) 
  [rows cols slices] = size(image);
else
  [rows cols] = size(image);
end;

% check for `flat' images
maxval = max(max(image));
minval = min(min(image));
if (minval==maxval) 
    disp(['WARNING: matrix minimum and maximum are equal (', ...
	   num2str(minval),')... I do not display flat images!']);
    return;
end;

% compose a `unique' directory name using clock and create it
t = clock;
dir_name = ['/tmp/ml.',num2str(t(5)),'s',num2str(t(6))];
eval(['!mkdir ', dir_name]);

% now compose the complete file name
fname = [dir_name,'/',fname];

% open file and write the image as doubles and close
fid = fopen(fname,'wb');
fwrite(fid,image,'double');
fclose(fid);

% create shell command to run xdisp
xdisp_cmd=['!xdisp ', fname, ...
	   ' -w ', int2str(rows), ...
	   ' -h ', int2str(cols), ...
	   ' -double ' ...
	   cmd_opts, ...
	   xtra_cmd_opts, ...
	   ' ; ' ...
	   '\rm -f ' , fname , ' ; '...
	   '\rmdir ', dir_name ' & '];

% issue shell command 
% disp(xdisp_cmd); % debug
eval(xdisp_cmd);

