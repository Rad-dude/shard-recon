# MRtrix3 module for motion and distortion correction.

This module contains the SHARD reconstruction software for slice-level motion 
correction in multi-shell diffusion MRI, as described in

Daan Christiaens, Lucilio Cordero-Grande, Maximilian Pietsch, Jana Hutter, Anthony N. Price, Emer J. Hughes, Katy Vecchiato, Maria Deprez, A. David Edwards, Joseph V.Hajnal, and J-Donald Tournier, *Scattered slice SHARD reconstruction for motion correction in multi-shell diffusion MRI*, NeuroImage 117437 (2020). doi:[10.1016/j.neuroimage.2020.117437](https://doi.org/10.1016/j.neuroimage.2020.117437)


## Setup & build

Prerequisites: MRtrix 3.0, Eigen 3.3.3, and python 3.x with numpy and scipy.

The code is built like any other [MRtrix3](https://github.com/MRtrix3/mrtrix3) 
module, i.e., by setting up a symbolic link to the core build script:

```
$ git clone https://github.com/dchristiaens/shard-recon.git
$ cd shard-recon
$ ln -s /path/to/mrtrix3/build
$ ln -s /path/to/mrtrix3/bin/mrtrix3.py bin/
$ ./build
```

This will compile the code into `shard-recon/bin`, which then needs to be added 
to the `PATH`.


## Help & support

Contact daan.christiaens@kcl.ac.uk

### Changes by AR:
- This branch contains the built version of the code on Linux Mint 20.1, built against MRTrix3 v. 3.0.2-78-g430465d6
- dwimotioncorrect is updated to use MRTrix3 v 3.0.3-296-g8a71cd60, specifically dwi2mask b02template with the UKBB_fMRI modified template included with KUL_NIS (KUL Neuroimaging tools)
- This version is not yet tested
