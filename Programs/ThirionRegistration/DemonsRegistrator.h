
#ifndef _DemonsRegistrator_h
#define _DemonsRegistrator_h

#include "itkObject.h"
#include "itkVector.h"
#include "itkImage.h"
#include "itkMultiResolutionPDEDeformableRegistration.h"
#include "itkRecursiveMultiResolutionPyramidImageFilter.h"

#include "itkArray.h"

namespace itk
{

  /** \class DemonsRegistrator
   *
   * This component computes the transform to register a
   * moving image onto a fixed image.
   *
   * In particular, it uses the deformable demons registration
   * algorithm.
   *
   * The registration is done using a multiresolution strategy.
   * At each resolution level, the downsampled images are obtained
   * using a RecursiveMultiResolutionPyramidImageFilter.
   * 
   * \warning This class requires both images to be 3D.
   * It can write out the deformation field and the checker board image 
   * of the fixed and output image.
   *
   * The registration process is activated by method Execute().
   *
   * Inputs:
   *   - pointer to fixed image
   *   - pointer to moving image
   *   - number of resolution levels
   *   - number of optimization iterations at each level
   *   - the initial rigid (quaternion) transform parameters
   *   - the coarest level shrink factors for the fixed image
   *   - the coarest level shrink factors for the moving image
   *
   * Outputs:
   *   - output deformation field
   *   - output image
   *   - Checkerboard image
   *   - x,y,z components of displacement fields.
   */ 
  template <
    typename TRealImage, 
    typename TOutputImage,
    typename TFieldValue = float>
  class DemonsRegistrator : public Object
  {
  public:

    /** Standard class typedefs. */
    typedef DemonsRegistrator Self;
    typedef Object Superclass;
    typedef SmartPointer<Self> Pointer;
    typedef SmartPointer<const Self>  ConstPointer;

    /** Run-time type information (and related methods). */
    itkTypeMacro(DemonsRegistrator, Object);

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Fixed Image Type. */
    typedef TRealImage RealImageType;

    /** Moving Image Type. */
    typedef TOutputImage OutputImageType;
    typedef typename OutputImageType::PixelType PixelType;

    /** Image dimension enumeration. */
    itkStaticConstMacro(ImageDimension, unsigned int, TRealImage::ImageDimension);

    /** Type to hold the number of checker boxes per dimension */
    typedef FixedArray< unsigned int,
                        ::itk::GetImageDimension<TRealImage>::ImageDimension >  PatternArrayType;

    /** Set Checker pattern */
    itkSetMacro( CheckerBoardPattern, PatternArrayType );
    itkGetConstReferenceMacro( CheckerBoardPattern, PatternArrayType );


    /** Deformation field value type. */
    typedef TFieldValue FieldValueType;

    /** Deformation field pixel type. */
    typedef Vector<FieldValueType,itkGetStaticConstMacro(ImageDimension)> FieldPixelType;

    /** Deformation field type. */
    typedef Image<FieldPixelType,itkGetStaticConstMacro(ImageDimension)> DeformationFieldType;

    /** Fixed Image Pyramid Type. */
    typedef RecursiveMultiResolutionPyramidImageFilter<
      RealImageType,
      RealImageType  >    FixedImagePyramidType;

    /** Moving Image Pyramid Type. */
    typedef RecursiveMultiResolutionPyramidImageFilter<
      RealImageType,
      RealImageType  >   MovingImagePyramidType;

    /** Registration Method. */
    typedef MultiResolutionPDEDeformableRegistration< 
      RealImageType, 
      RealImageType,
      DeformationFieldType >    RegistrationType;

    /** UnsignedIntArray type. */
    typedef Array<unsigned int> UnsignedIntArray;

    /** ShrinkFactorsArray type. */
    typedef FixedArray<unsigned int,itkGetStaticConstMacro(ImageDimension)> ShrinkFactorsArray;

    /** Set the fixed image. */
    itkSetObjectMacro( FixedImage, RealImageType );

    /** Set the moving image. */
    itkSetObjectMacro( MovingImage, RealImageType );


    /** Set the Unnormalized moving image. */
    itkSetObjectMacro( UnNormalizedMovingImage, RealImageType );

    /** Set the Unnormalized moving image. */
    itkSetObjectMacro( UnNormalizedFixedImage, RealImageType );

    /** Set the number of resolution levels. */
    itkSetClampMacro( NumberOfLevels, unsigned short, 1,
                      NumericTraits<unsigned short>::max() );

    /** Set the number of iterations per level. */
    itkSetMacro( NumberOfIterations, UnsignedIntArray );

    /** Set the fixed and moving image shrink factors. */
    itkSetMacro( FixedImageShrinkFactors, ShrinkFactorsArray );
    itkSetMacro( MovingImageShrinkFactors, ShrinkFactorsArray );

    /** Set Displacementname */
    itkSetStringMacro( DisplacementBaseName );
    itkGetStringMacro( DisplacementBaseName );

    /** Set WarpedImageName */
    itkSetStringMacro( WarpedImageName );
    itkGetStringMacro( WarpedImageName );

    /** Set CheckerBoard ImageName */
    itkSetStringMacro(CheckerBoardFileName);
    itkGetStringMacro(CheckerBoardFileName);

    /** Set Deformation field output file Name */
    itkSetStringMacro(DeformationFieldOutputName);
    itkGetStringMacro(DeformationFieldOutputName);

    /** Method to execute the registration. */
    virtual void Execute();

    /** Get the deformation field. */
    itkGetObjectMacro( DeformationField, DeformationFieldType );

    /** Initialize registration at the start of new level. */
    void StartNewLevel();

    /**Output Normalized Image.*/
    itkSetStringMacro(OutNormalized );
    itkGetStringMacro( OutNormalized );

    /**Set Debug mode*/
    itkSetStringMacro(OutDebug );
    itkGetStringMacro( OutDebug );

    itkSetStringMacro (InitialTransformFileName);
    itkGetStringMacro (InitialTransformFileName);

    itkSetMacro(DefaultPixelValue, typename RealImageType::PixelType);
    itkGetMacro(DefaultPixelValue, typename RealImageType::PixelType);

  protected:
    DemonsRegistrator();
    ~DemonsRegistrator();

  private:
    DemonsRegistrator( const Self& ); //purposely not implemented
    void operator=( const Self& ); //purposely not implemented

    void WriteDisplacementComponents();
    typename RealImageType::Pointer             m_FixedImage;
    typename RealImageType::Pointer             m_MovingImage;
    typename RealImageType::Pointer             m_UnNormalizedMovingImage;
    typename RealImageType::Pointer             m_UnNormalizedFixedImage;
    typename FixedImagePyramidType::Pointer     m_FixedImagePyramid;
    typename MovingImagePyramidType::Pointer    m_MovingImagePyramid;
    typename RegistrationType::Pointer          m_Registration;
    typename RealImageType::PixelType           m_DefaultPixelValue;

    unsigned short                              m_NumberOfLevels;
    UnsignedIntArray                            m_NumberOfIterations;

    ShrinkFactorsArray                          m_MovingImageShrinkFactors;
    ShrinkFactorsArray                          m_FixedImageShrinkFactors;

    typename DeformationFieldType::Pointer          m_DeformationField;
    std::string m_InitialTransformFileName;
    //    typename DeformationFieldType::Pointer m_InitialDeformationField;
    unsigned long                               m_Tag;
    std::string                   m_DisplacementBaseName;
    std::string                   m_WarpedImageName;
    std::string                   m_CheckerBoardFileName;
    std::string                   m_DeformationFieldOutputName;
    PatternArrayType              m_CheckerBoardPattern;
    std::string                   m_OutNormalized ;
    std::string                   m_OutDebug ;

  };

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "DemonsRegistrator.txx"
#endif

#endif
