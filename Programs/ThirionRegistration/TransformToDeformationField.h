#ifndef TransformToDeformationField_h
#define TransformToDeformationField_h
#include "itkIO.h"
//#include "CrossOverAffineSystem.h"

/** 
 * Go from any subclass of Transform, to the corresponding deformation field
 */

template<typename DeformationFieldType,typename TransformType>
typename DeformationFieldType::Pointer
TransformToDeformationField
(int *size, double *spacing, double *origin,
 typename TransformType::Pointer &xfrm)
{
  typedef typename DeformationFieldType::PixelType DeformationPixelType;

  typename DeformationFieldType::DirectionType RIPDir=itk::SpatialOrientationAdapter().ToDirectionCosines(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
  //Orientation must be in RIP to match the xfrm.
  typename DeformationFieldType::Pointer deformation =
    itkUtil::ImageReinitialize<DeformationFieldType>(size[0],spacing[0],
                                                     size[1],spacing[1],
                                                     size[2],spacing[2],
                                                     RIPDir);
  for(int z = 0; z < size[2]; z++)
    {
    for(int y = 0; y < size[1];y++)
      {
      for(int x = 0; x < size[0]; x++)
        {
        itk::Point<double,3> a, b;
        a[0] = static_cast<double>(x) * spacing[0] + origin[0];
        a[1] = static_cast<double>(y) * spacing[1] + origin[1];
        a[2] = static_cast<double>(z) * spacing[2] + origin[2];
        b = xfrm->TransformPoint(a);
        DeformationPixelType p;
        p[0] = b[0] - a[0];
        p[1] = b[1] - a[1];
        p[2] = b[2] - a[2];
        typename DeformationFieldType::IndexType ind;
        ind[0] = x;
        ind[1] = y;
        ind[2] = z;
        deformation->SetPixel(ind,p);
        }
      }
    }
  return deformation;
}

template<typename DeformationFieldType, typename TransformType>
typename DeformationFieldType::Pointer
TransformToDeformationField
(const typename DeformationFieldType::RegionType::SizeType &size,
 const typename DeformationFieldType::SpacingType spacing,
 const typename DeformationFieldType::PointType origin,
 typename TransformType::Pointer &xfrm)
{
  int _size[3];
  double _spacing[3];
  double _origin[3];

  for(unsigned i = 0; i < 3; i++)
    {
    _size[i] = size[i];
    _spacing[i] = spacing[i];
    _origin[i] = origin[i];
    }
  return TransformToDeformationField<DeformationFieldType,TransformType>
    (_size,_spacing,_origin, xfrm);
}

template<typename DeformationFieldType,typename TransformType>
typename DeformationFieldType::Pointer
TransformToDeformationField
(int *size, double *spacing,
 typename TransformType::Pointer &xfrm)
{
  typedef typename DeformationFieldType::PixelType DeformationPixelType;

  typename DeformationFieldType::DirectionType RIPDir=itk::SpatialOrientationAdapter().ToDirectionCosines(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
  //Orientation must be in RIP to match the xfrm.
  typename DeformationFieldType::Pointer deformation =
    itkUtil::ImageReinitialize<DeformationFieldType>(size[0],spacing[0],
                                                     size[1],spacing[1],
                                                     size[2],spacing[2],
                                                     RIPDir);
  for(int z = 0; z < size[2]; z++)
    {
    for(int y = 0; y < size[1];y++)
      {
      for(int x = 0; x < size[0]; x++)
        {
        itk::Point<double,3> a, b;
        a[0] = x;
        a[1] = y;
        a[2] = z;
        b = xfrm->TransformPoint(a);
        DeformationPixelType p;
        p[0] = b[0] - a[0];
        p[1] = b[1] - a[1];
        p[2] = b[2] - a[2];
        typename DeformationFieldType::IndexType ind;
        ind[0] = x;
        ind[1] = y;
        ind[2] = z;
        deformation->SetPixel(ind,p);
        }
      }
    }
  return deformation;
}

template<typename DeformationFieldType, typename TransformType>
typename DeformationFieldType::Pointer
TransformToDeformationField
(const typename DeformationFieldType::RegionType::SizeType &size,
 const typename DeformationFieldType::SpacingType spacing,
 typename TransformType::Pointer &xfrm)
{
  int _size[3];
  double _spacing[3];
  for(unsigned i = 0; i < 3; i++)
    {
    _size[i] = size[i];
    _spacing[i] = spacing[i];
    }
  return TransformToDeformationField<DeformationFieldType,TransformType>
    (_size,_spacing,xfrm);
}
/*
template<typename DeformationFieldType>
typename DeformationFieldType::Pointer
AffineTransformToDeformationField
(int *size, double *spacing, 
 typename CrossOverAffineSystem<double,3>::AffineTransformPointer &xfrm)
{
  return 
    TransformToDeformationField
    <DeformationFieldType,CrossOverAffineSystem<double,3>::AffineTransformType>
    (size,spacing,xfrm);
    
}

template<typename DeformationFieldType>
typename DeformationFieldType::Pointer
AffineTransformToDeformationField
(const typename DeformationFieldType::RegionType::SizeType &size,
 const typename DeformationFieldType::SpacingType spacing,
 typename CrossOverAffineSystem<double,3>::AffineTransformPointer &xfrm)
{
  return
    TransformToDeformationField
    <DeformationFieldType,CrossOverAffineSystem<double,3>::AffineTransformType>
    (size,spacing,xfrm);
}
*/
#endif // TransformToDeformationField_h
