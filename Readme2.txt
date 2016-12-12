XParser 0.17.1
This version of xparser was released by Chris Greenough in Feb. 2014.

It contains:
- Support for GNU GSL library for statistical functions
- System and FLAME environment variables (see Readme.txt):

1. System environment: FLAME_XPARSER_DIR: If this is set, xparser assumes the flame templates are in that folder. If not set, xparser looks in the current folder for templates.

2. Flame environment constant in model.xml to set the random seed: GSL_RNG_SEED.
If this constant is present in model.xml, then the Makefile_templ will include code to link in the GSL libraries. The RNG is initialized by code in main.templ as replacement for the standard RNG of C.

3. Setting the GSL random seed depends on whether xparser is run in debug or production mode:
a. In debug mode: value depends on user-defined constant GSL_RNG_SEED:
a1. If GSL_RNG_SEED == 0: set gsl_seed=gsl_rng_default_seed (0 by default).
a2. If GSL_RNG_SEED =!= 0: set gsl_seed=GSL_RNG_SEED (user-defined).
b. In production mode: value depends on value of gsl_rng_default_seed:
b1. If gsl_rng_default_seed ==0: gsl_seed is initialized on time (default).
b2. If gsl_rng_default_seed =!= 0: gsl_seed=gsl_rng_default_seed (see 4. below)

4. System environment: GSL_RNG_TYPE and GSL_RNG_SEED
- Without any environment variables, GSL uses the initial defaults, an mt19937 generator with a default seed of gsl_rng_default_seed=0.
- See also: https://www.gnu.org/software/gsl/manual/html_node/Random-number-environment-variables.html
