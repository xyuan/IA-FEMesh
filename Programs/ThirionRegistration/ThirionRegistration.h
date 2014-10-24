#ifndef _ThirionRegistration_h
#define _ThirionRegistration_h

#include <string>
#include "DemonsPreprocessor.h"
#include "DemonsRegistrator.h"
#include "ValidationInputParser.h"
#include "ApplicationBase.h"
#include "itkCheckerBoardImageFilter.h"
namespace itk
{

/*This file defines Thirion registration class which initializes the input parser, preprocessor and the registrator. */

  template < typename TImage,
    typename TRealImage, typename TOutputImage >
    class ThirionRegistration:public ApplicationBase <
    ValidationInputParser < TImage >,
    DemonsPreprocessor < TImage, TRealImage >,
    DemonsRegistrator < TRealImage, TOutputImage > >
  {
  public:

     /** Standard class typedefs. */
    typedef ThirionRegistration Self;
    typedef ApplicationBase <
      ValidationInputParser < TImage >,
      DemonsPreprocessor < TImage, TRealImage >,
      DemonsRegistrator < TRealImage, TRealImage > >Superclass;
    typedef SmartPointer < Self > Pointer;
    typedef SmartPointer < const Self > ConstPointer;

    /** Deformation field pixel type. */
    typedef float FieldValueType;
    typedef Vector<FieldValueType,3> FieldPixelType;
    typedef Image<FieldPixelType,3> DeformationFieldType;

     /** Run-time type information (and related methods). */
      itkTypeMacro (ThirionRegistration, ApplicationBase);

     /** Method for creation through the object factory. */
      itkNewMacro (Self);

     /** Image types. */
    typedef TImage ImageType;
    typedef TRealImage RealImageType;

     /** Image dimension. */
      itkStaticConstMacro (ImageDimension, unsigned int,
         TImage::ImageDimension);

     /** Type to hold the number of checker boxes per dimension */
    typedef FixedArray < unsigned int,::itk::GetImageDimension <
      TImage >::ImageDimension > PatternArrayType;

    typedef typename ImageType::PixelType PixelType;
    typedef typename ImageType::IndexType IndexType;
    typedef typename ImageType::SizeType SizeType;


     /** Set the atlas patient ID. */
      itkSetStringMacro (TheMovingImageFileName);
      itkGetStringMacro (TheMovingImageFileName);

     /** Set the subject patient ID. */
      itkSetStringMacro (TheFixedImageFileName);
      itkGetStringMacro (TheFixedImageFileName);

     /** Set Displacementname */
      itkSetStringMacro (DisplacementBaseName);
      itkGetStringMacro (DisplacementBaseName);
     /** Set WarpedImageName */
      itkSetStringMacro (WarpedImageName);
      itkGetStringMacro (WarpedImageName);

     /** Set input parameter file */
      itkSetStringMacro (ParameterFileName);

     /** Set output transformation filename. */
      itkSetStringMacro (OutputFileName);

     /**Set checker board Image filename*/
      itkSetStringMacro (CheckerBoardFileName);
      itkGetStringMacro (CheckerBoardFileName);

     /**Set Deformation field output filename*/
      itkSetStringMacro (DeformationFieldOutputName);
      itkGetStringMacro (DeformationFieldOutputName);

     /** Set Checker pattern */
      itkSetMacro (CheckerBoardPattern, PatternArrayType);
      itkGetConstReferenceMacro (CheckerBoardPattern, PatternArrayType);

     /** Set append output file boolean. */
      itkSetMacro (AppendOutputFile, bool);
      itkGetMacro (AppendOutputFile, bool);
      itkBooleanMacro (AppendOutputFile);

    /*BOBF macros */
     /**Set Target Mask filename*/
      itkSetStringMacro (BOBFTargetMask);
      itkGetStringMacro (BOBFTargetMask);

     /**Set Template Mask filename*/
      itkSetStringMacro (BOBFTemplateMask);
      itkGetStringMacro (BOBFTemplateMask);

     /**Output Normalized Image.*/
      itkSetStringMacro (OutNormalized);
      itkGetStringMacro (OutNormalized);

     /**Set Debug mode*/
      itkSetStringMacro (OutDebug);
      itkGetStringMacro (OutDebug);

     /** Set/Get the lower threshold. The default is 0. */
      itkSetMacro (Lower, PixelType);
      itkGetMacro (Lower, PixelType);

     /** Set/Get the upper threshold. The default is 70 */
      itkSetMacro (Upper, PixelType);
      itkGetMacro (Upper, PixelType);

     /** Set/Get value to replace thresholded pixels. Pixels that lie *
                  *  within Lower and Upper (inclusive) will be replaced with this
                  *  value. The default is 1. */
      itkSetMacro (DefaultPixelValue, PixelType);
      itkGetMacro (DefaultPixelValue, PixelType);

     /** Set the radius of the neighborhood used for a mask. */
      itkSetMacro (Radius, SizeType);
     /** Get the radius of the neighborhood used to compute the median */
      itkGetConstReferenceMacro (Radius, SizeType);

     /** Set the Seed of the neighborhood used for a mask. */
      itkSetMacro (Seed, IndexType);
     /** Get the radius of the neighborhood used to compute the median */
      itkGetConstReferenceMacro (Seed, IndexType);

    /** Set the Input Landmark Filename*/
    itkSetMacro(MedianFilterRadius,  int);
    itkGetMacro(MedianFilterRadius,  int);

    /** Set the initial deformation field to prime registration */
    //    itkSetObjectMacro(InitialDeformationField,DeformationFieldType);
    itkSetStringMacro (InitialTransformFileName);
    itkGetStringMacro (InitialTransformFileName);
  protected:

      ThirionRegistration ();
      virtual ~ ThirionRegistration ()
    {
    };

    /** Initialize the input parser. */
    virtual void InitializeParser ();

    /*** Initialize the preprocessor */
    virtual void InitializePreprocessor ();

    /*** Initialize the registrator  */
    virtual void InitializeRegistrator ();


  private:

    std::string m_TheMovingImageFileName;
    std::string m_TheFixedImageFileName;
    std::string m_DisplacementBaseName;
    std::string m_WarpedImageName;

    std::string m_ParameterFileName;
    std::string m_OutNormalized;
    std::string m_OutDebug;
    std::string m_OutputFileName;
    std::string m_CheckerBoardFileName;
    std::string m_DeformationFieldOutputName;
    bool m_AppendOutputFile;
    PatternArrayType m_CheckerBoardPattern;
    std::string m_BOBFTargetMask;
    std::string m_BOBFTemplateMask;
    IndexType m_Seed;
    PixelType m_Lower;
    PixelType m_Upper;
    PixelType m_DefaultPixelValue;
    SizeType m_Radius;
    int m_MedianFilterRadius;
    std::string m_InitialTransformFileName;
    //typename DeformationFieldType::Pointer m_InitialDeformationField;
  };

}        // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "ThirionRegistration.txx"
#endif

#endif

