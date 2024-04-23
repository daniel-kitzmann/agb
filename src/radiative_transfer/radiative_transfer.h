 
#ifndef _radiative_transfer_h
#define _radiative_transfer_h

#include <vector>
#include <string>
#include <iostream>

#include "../additional/tri_diagonal_matrix.h"

namespace agb {

//forward declaration
class ModelConfig;
class SpectralGrid;
class Atmosphere;



struct AnglePoint
{
  AnglePoint(const size_t nb_spectral_points)
  {
    u.assign(nb_spectral_points, 0.0);
    intensity_in.assign(nb_spectral_points, 0.0);
    intensity_out.assign(nb_spectral_points, 0.0);
  }

  std::vector<double> u;
  double angle;
  double quadrature_weight;
  std::vector<double> intensity_out;
  std::vector<double> intensity_in;
};


struct zPoint
{
  double z = 0;
  double radius = 0;
  size_t radius_index = 0;
  AnglePoint* angle_point = nullptr;
};



class ImpactParam{
  public:
    std::vector<zPoint> z_grid;
    double p = 0;
    size_t nb_z_points = 0;

    void solveRadiativeTransfer(
      const size_t nu,
      const double boundary_planck_derivative,
      const double boundary_flux_correction,
      const std::vector<double>& extinction_coeff,
      const std::vector<double>& source_function);
  private:
    std::vector<double> opticalDepth(
      const std::vector<double>& extinction_coeff);
    void assembleSystem(
      const std::vector<double>& optical_depth,
      const std::vector<double>& source_function,
      const double boundary_planck_derivative,
      const double boundary_flux_correction,
      const double boundary_exctinction_coeff,
      aux::TriDiagonalMatrix& M,
      std::vector<double>& rhs);
};



struct RadiationField
{
  RadiationField(const size_t nb_spectral_points);

  std::vector<double> eddington_flux;
  std::vector<double> mean_intensity;
  std::vector<double> eddington_factor;
  std::vector<AnglePoint> angle_grid;

  std::vector<double> mean_intensity_impact;
  std::vector<double> eddington_k_impact;
  std::vector<double> eddington_flux_impact;

  std::vector<double> angles;
  
  size_t radius_index = 0;
  size_t nb_angles = 0;

  void createAngleGrid(
    std::vector<ImpactParam>& impact_parameter_grid,
    const double radius,
    const size_t radius_index_);
  
  void angularIntegration();
};



class RadiativeTransfer{
  public:
    RadiativeTransfer(
      ModelConfig* config_,
      SpectralGrid* spectral_grid_,
      Atmosphere* atmosphere_);
    ~RadiativeTransfer() {}

    void solveRadiativeTransfer();
  protected:
    ModelConfig* config;
    SpectralGrid* spectral_grid;
    Atmosphere* atmosphere;

    const size_t nb_core_impact_param = 0;
    const size_t nb_impact_param = 0;

    const size_t nb_grid_points = 0;
    const size_t nb_spectral_points = 0;

    std::vector<ImpactParam> impact_parameter_grid;
    std::vector<RadiationField> radiation_field;

    std::vector<std::vector<double>> eddington_factor_f;
    std::vector<std::vector<double>> eddington_factor_h;
    std::vector<std::vector<double>> sphericality_factor;

    std::vector<std::vector<double>> extinction_coeff;
    std::vector<std::vector<double>> source_function;

    void createImpactParameterGrid();
    void createZGrids();
    void calcSourceFunction();
    double boundaryFluxCorrection();
    void calcEddingtonFactors();
    void calcSphericalityFactor();
};



}

#endif