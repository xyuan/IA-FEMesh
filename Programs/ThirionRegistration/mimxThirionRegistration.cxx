#include <itkBSplineDeformableTransform.h>
#include <itkTransformFactory.h>
#include <itkVersorRigid3DTransform.h>
#include <itkThinPlateR2LogRSplineKernelTransform.h>


extern int ThrionRegistrationPrimary(int argc, char * argv[]);



//main function built in ThrionRegistionPrimary.cxx so that testing only builds templates once.
int main(int argc, char * argv[])
{
  typedef double BSplineCoordinateRepType;
  typedef itk::VersorRigid3DTransform< double >     RigidTransformType;
  typedef itk::BSplineDeformableTransform<
                            BSplineCoordinateRepType,
                            3,
                            3 >     BSplineTransformType;
  typedef itk::ThinPlateR2LogRSplineKernelTransform<double,3>  ThinPlateSplineTransformType;
  itk::TransformFactory<BSplineTransformType>::RegisterTransform();
  itk::TransformFactory<RigidTransformType>::RegisterTransform();
  itk::TransformFactory<ThinPlateSplineTransformType>::RegisterTransform();

  return ThrionRegistrationPrimary(argc,argv);
}
