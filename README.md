# CEGA
We developed maximum likelihood method for detecting positive selection or balancing selection using multilocus or genomic polymorphism and divergence data from two species. The method is especially useful for investigating natural selection in noncoding regions.

If you use CEGA and publish your analysis, please cite the publication:
Zhao, S., Chi, L. & Chen, H. CEGA: a method for inferring natural selection by comparative population genomic analysis across species. Genome Biol 24, 219 (2023). https://doi.org/10.1186/s13059-023-03068-8

To run CEGA, enter the following command (see CEGA v1.2 user manual for details):

./CEGA [arguments]

Inputs:

-i1	population 1 genetic variant file (.vcf .vcf.gz .tped .hap ).

-p1	population 1 position file (format: chr position, split by tab), only required for .hap (-i1) genetic variant file.

-i2	population 2 genetic variant file (.hap .vcf .vcf.gz .tped).

-p2	population 2 position file (format: chr position, split by tab), only required for .hap (-i2) genetic variant file.

-o	output file name.

Options:

-N0 	(double) initial lower bound upper bound (defalut: 10000.0 100.0 1000000.0).

Set the initial value and range of haploid effective population size for common ancestor species. CEGA will estimate N0 under these constraints. For two species of long-term divergence time, providing additional information on N0 can help to infer global parameters more accurately. Especially, N0 can be fixed by setting the same values of the low bound and up bound. 

-N1	(double) initial lower bound upper bound (defalut: 10000.0 100.0 1000000.0).

Similar to -N0. CEGA can infer N1 from data reasonably, and it is not recommended to set the constraints.

-N2	(double) initial lower bound upper bound (defalut: 10000.0 100.0 1000000.0).

Similar to -N0. CEGA can inter N2 from data reasonably, and it is not recommended to set the constraints.

-T	(double) initial lower bound upper bound (defalut:10000.0 100.0 10000000.0).

Set the initial value and range of divergence time. CEGA will estimate T under these constraints. For two species of long-term divergence time, providing additional information on T can help to infer global parameters more accurately. 

-t	(int) thread number (default: 1).

-d	(int) filtering windows with s1+s2+s12+D < this value (default: 0).

-mu 	(double) mutation rate (default:2.5e-8). Unit: per base per generation.

-ws	(int) window_size step_size (default: 10000 1000). Unit: bp.

Set the window size and step size (unit: bp). The first window of each chromosome starts from the first SNP.

-wf 	(file) window file (format: chr start (1-base, include) end (1-base, include) effective_length, split by tab), if input, '-ws' is disable (default: null).

The argument -wf is a substitute for -ws. Set the windows by providing a detailed window information file. The window information file provides effective window sizes, denoting the remaining genomic length after filtering. See details on the file format in the “Input Files” section.

-wf_g	(file) window file to specify neutral genome region for estimating global parameters, format same to '-wf' (default: null).

Set the subset of the genomic regions applied in estimating the global parameters. If not input, CEGA will estimate the global parameters using the complete genomic information in input files. See details on the file format in the “Input Files” section.  

-LRT	(int)1: implement CEGA-LRT, 0: implement CEGA-λ (default:0)

CEGA provides two methods to do significant test: the distribution of λ (CEGA-λ), and the likelihood ratio test (CEGA-LRT). By default, CEGA implement CEGA-λ.
