/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: ModelToImageRegistration1.cxx,v $
  Language:  C++
  Date:      $Date: 2006/09/07 13:28:53 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include <itkImageToSpatialObjectRegistrationMethod.h>
#include <itkImageToSpatialObjectMetric.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkEuler2DTransform.h>
#include <itkOnePlusOneEvolutionaryOptimizer.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkNormalVariateGenerator.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkCommand.h>
#include <metaCommand.h>



template < class TOptimizer >
class IterationCallback : public itk::Command 
{
public:
  typedef IterationCallback   Self;
  typedef itk::Command  Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;
  
  itkTypeMacro( IterationCallback, Superclass );
  itkNewMacro( Self );

  /** Type defining the optimizer. */
  typedef    TOptimizer     OptimizerType;

  /** Method to specify the optimizer. */
  void SetOptimizer( OptimizerType * optimizer )
    { 
      m_Optimizer = optimizer;
      m_Optimizer->AddObserver( itk::IterationEvent(), this );
    }

  /** Execute method will print data at each iteration */
  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object *, const itk::EventObject & event)
    {
      if( typeid( event ) == typeid( itk::StartEvent ) )
        {
        std::cout << std::endl << "Position              Value";
        std::cout << std::endl << std::endl;
        }    
      else if( typeid( event ) == typeid( itk::IterationEvent ) )
        {
        std::cout << m_Optimizer->GetCurrentIteration() << "   ";
        std::cout << m_Optimizer->GetValue() << "   ";
        std::cout << m_Optimizer->GetCurrentPosition() << std::endl;
        }
      else if( typeid( event ) == typeid( itk::EndEvent ) )
        {
        std::cout << std::endl << std::endl;
        std::cout << "After " << m_Optimizer->GetCurrentIteration();
        std::cout << "  iterations " << std::endl;
        std::cout << "Solution is    = " << m_Optimizer->GetCurrentPosition();
        std::cout << std::endl;
        }
    }

protected:
  IterationCallback() {};
  itk::WeakPointer<OptimizerType>   m_Optimizer;
};


template <typename TFixedImage, typename TMovingSpatialObject>
class SimpleImageToSpatialObjectMetric : 
  public itk::ImageToSpatialObjectMetric<TFixedImage,TMovingSpatialObject>
{
//  Software Guide : EndCodeSnippet 
public:
  /** Standard class typedefs. */
  typedef SimpleImageToSpatialObjectMetric  Self;
  typedef itk::ImageToSpatialObjectMetric<TFixedImage,TMovingSpatialObject>  
  Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;

  typedef itk::Point<double,3>   PointType;
  typedef std::list<PointType> PointListType;
  typedef TMovingSpatialObject MovingSpatialObjectType;
  typedef typename Superclass::ParametersType ParametersType;
  typedef typename Superclass::DerivativeType DerivativeType;
  typedef typename Superclass::MeasureType    MeasureType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(SimpleImageToSpatialObjectMetric, ImageToSpatialObjectMetric);

  itkStaticConstMacro( ParametricSpaceDimension, unsigned int, 3 );

  /** Specify the moving spatial object. */
  void SetMovingSpatialObject( const MovingSpatialObjectType * object)
    {
      if(!this->m_FixedImage)
        {
        std::cout << "Please set the image before the moving spatial object" << std::endl;
        return;
        }
      this->m_MovingSpatialObject = object;
      m_PointList.clear();
      typedef itk::ImageRegionConstIteratorWithIndex<TFixedImage> myIteratorType;

      myIteratorType it(this->m_FixedImage,this->m_FixedImage->GetBufferedRegion());

      itk::Point<double,3> point;

      while( !it.IsAtEnd() )
        {
        this->m_FixedImage->TransformIndexToPhysicalPoint( it.GetIndex(), point );

        if(this->m_MovingSpatialObject->IsInside(point,99999))
          { 
          m_PointList.push_back( point );
          }    
        ++it;
        }

      std::cout << "Number of points in the metric = " << static_cast<unsigned long>( m_PointList.size() ) << std::endl;
    }

  unsigned int GetNumberOfParameters(void) const  {return ParametricSpaceDimension;};

  /** Get the Derivatives of the Match Measure */
  void GetDerivative( const ParametersType &, DerivativeType & ) const
    {
      return;
    }

  /** Get the value for SingleValue optimizers. */
  MeasureType    GetValue( const ParametersType & parameters ) const
    {   
      double value;
      this->m_Transform->SetParameters( parameters );
    
      PointListType::const_iterator it = m_PointList.begin();
    
      typename TFixedImage::SizeType size =
        this->m_FixedImage->GetBufferedRegion().GetSize();

      itk::Index<3> index;
      itk::Index<3> start = this->m_FixedImage->GetBufferedRegion().GetIndex();

      value = 0;
      while( it != m_PointList.end() )
        {
        PointType transformedPoint = this->m_Transform->TransformPoint(*it);
        if( this->m_Interpolator->IsInsideBuffer( transformedPoint ) )
          {
          value += this->m_Interpolator->Evaluate( transformedPoint );
          }
        it++;
        }
      return value;
    }


  /** Get Value and Derivatives for MultipleValuedOptimizers */
  void GetValueAndDerivative( const ParametersType & parameters,
       MeasureType & Value, DerivativeType  & Derivative ) const
    {
      Value = this->GetValue(parameters);
      this->GetDerivative(parameters,Derivative);
    }

private:
  PointListType m_PointList;
};


//  Software Guide : BeginLatex
//  
//  Having defined all the registration components we are ready to put the
//  pieces together and implement the registration process.
//
//  Software Guide : EndLatex 


int main( int argc, char *argv[] )
{
  MetaCommand command;

  command.SetOption("SubjectMaskFile","s",false,"Subject Mask Filename");
  command.AddOptionField("CanonicalSurface","filename",MetaCommand::STRING,true);

  command.SetOption("CanonicalMaskFile","c",false,"Canonical Mask Filename");
  command.AddOptionField("SubjectSurface","filename",MetaCommand::STRING,true);
  
  command.SetOption("OutputSurface","o",false,"Output Surface File");
  command.AddOptionField("OutputSurface","filename",MetaCommand::STRING,true);

  command.SetOption("CanonicalMesh","m",false,"Input Canonical Mesh File");
  command.AddOptionField("CanonicalMesh","filename",MetaCommand::STRING,true);

  command.SetOption("OutputMesh","x",false,"Output Canonical Mesh File");
  command.AddOptionField("OutputMesh","filename",MetaCommand::STRING,true);
 
  command.SetOption("NumberOfIterations","i",false,"Number of Iterations [100]");
  command.AddOptionField("NumberOfIterations","number",MetaCommand::INT,false,"100");
  
  command.SetOption("NumberOfLandmarks","n",false,"Number of Landmarks [100]");
  command.AddOptionField("NumberOfLandmarks","landmarks",MetaCommand::INT,false,"100");

  command.SetOption("MaximumMeanDistance","d",false,"Maximum Mean Distance");
  command.AddOptionField("MaximumMeanDistance","distance",MetaCommand::FLOAT,false,"1.0");

  //  Software Guide : BeginCodeSnippet
  typedef itk::GroupSpatialObject< 2 >     GroupType;
  typedef itk::EllipseSpatialObject< 2 >   EllipseType;
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  The image is instantiated in the following lines using the pixel
  //  type and the space dimension. This image uses a \code{float} pixel
  //  type since we plan to blur it in order to increase the capture radius of
  //  the optimizer. Images of real pixel type behave better under blurring
  //  than those of integer pixel type.
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  typedef itk::Image< float, 2 >      ImageType;
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  Here is where the fun begins! In the following lines we create the
  //  EllipseSpatialObjects using their \code{New()} methods, and
  //  assigning the results to SmartPointers. These lines will create
  //  three ellipses.
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  EllipseType::Pointer ellipse1 = EllipseType::New();
  EllipseType::Pointer ellipse2 = EllipseType::New();
  EllipseType::Pointer ellipse3 = EllipseType::New();
  //  Software Guide : EndCodeSnippet  


  //  Software Guide : BeginLatex
  //  
  //  Every class deriving from SpatialObject has particular
  //  parameters enabling the user to tailor its shape. In the case of the
  //  EllipseSpatialObject, \code{SetRadius()} is used to
  //  define the ellipse size. An additional \code{SetRadius(Array)} method
  //  allows the user to define the ellipse axes independently.
  //
  //  \index{itk::EllipseSpatialObject!SetRadius()}
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  ellipse1->SetRadius(  10.0  );
  ellipse2->SetRadius(  10.0  );
  ellipse3->SetRadius(  10.0  );
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  The ellipses are created centered in space by default. We use the
  //  following lines of code to arrange the ellipses in a triangle.
  //  The spatial transform intrinsically associated with the object is
  //  accessed by the \code{GetTransform()} method. This transform can define
  //  a translation in space with the \code{SetOffset()} method.  We take
  //  advantage of this feature to place the ellipses at particular
  //  points in space.
  //
  //  Software Guide : EndLatex 

  // Place each ellipse at the right position to form a triangle

  //  Software Guide : BeginCodeSnippet
  EllipseType::TransformType::OffsetType offset;
  offset[ 0 ] = 100.0;
  offset[ 1 ] =  40.0;

  ellipse1->GetObjectToParentTransform()->SetOffset(offset);
  ellipse1->ComputeObjectToWorldTransform();
 
  offset[ 0 ] =  40.0;
  offset[ 1 ] = 150.0;
  ellipse2->GetObjectToParentTransform()->SetOffset(offset);
  ellipse2->ComputeObjectToWorldTransform();

  offset[ 0 ] = 150.0;
  offset[ 1 ] = 150.0;
  ellipse3->GetObjectToParentTransform()->SetOffset(offset);
  ellipse3->ComputeObjectToWorldTransform();
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  Note that after a change has been made in the transform, the
  //  SpatialObject invokes the method
  //  \code{ComputeGlobalTransform()} in order to update its global
  //  transform.  The reason for doing this is that SpatialObjects
  //  can be arranged in hierarchies. It is then possible to change the
  //  position of a set of spatial objects by moving the parent of the group.
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginLatex
  //  
  //  Now we add the three EllipseSpatialObjects to a
  //  GroupSpatialObject that will be subsequently passed on to the
  //  registration method. The GroupSpatialObject facilitates the
  //  management of the three ellipses as a higher level structure
  //  representing a complex shape. Groups can be nested any number of levels
  //  in order to represent shapes with higher detail.
  //
  //  \index{itk::GroupSpatialObject!New()}
  //  \index{itk::GroupSpatialObject!Pointer}
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  GroupType::Pointer group = GroupType::New();
  group->AddSpatialObject( ellipse1 );
  group->AddSpatialObject( ellipse2 );
  group->AddSpatialObject( ellipse3 );
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  Having the geometric model ready, we proceed to generate the binary
  //  image representing the imprint of the space occupied by the ellipses.
  //  The SpatialObjectToImageFilter is used to that end. Note that
  //  this filter is instantiated over the spatial object used and the image
  //  type to be generated.
  //
  //
  //  \index{itk::Spatial\-Object\-To\-Image\-Filter!Instantiation}
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  typedef itk::SpatialObjectToImageFilter< GroupType, ImageType >   
    SpatialObjectToImageFilterType;
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  With the defined type, we construct a filter using the \code{New()}
  //  method. The newly created filter is assigned to a SmartPointer.
  //
  //  \index{itk::SpatialObjectToImageFilter!New()}
  //  \index{itk::SpatialObjectToImageFilter!Pointer}
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  SpatialObjectToImageFilterType::Pointer imageFilter = 
    SpatialObjectToImageFilterType::New();
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  The GroupSpatialObject is passed as input to the filter.
  //
  //  \index{itk::SpatialObjectToImageFilter!SetInput()}
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  imageFilter->SetInput(  group  );
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  The \doxygen{SpatialObjectToImageFilter} acts as a resampling filter.
  //  Therefore it requires the user to define the size of the desired output
  //  image. This is specified with the \code{SetSize()} method.
  //
  //  \index{itk::SpatialObjectToImageFilter!SetSize()}
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  ImageType::SizeType size;
  size[ 0 ] = 200;
  size[ 1 ] = 200;
  imageFilter->SetSize( size );
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  Finally we trigger the execution of the filter by calling the
  //  \code{Update()} method.
  //
  //  \index{itk::SpatialObjectToImageFilter!Update()}
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  imageFilter->Update();
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  In order to obtain a smoother metric, we blur the image using a
  //  \doxygen{DiscreteGaussianImageFilter}. This extends the capture radius
  //  of the metric and produce a more continuous cost function to
  //  optimize. The following lines instantiate the Gaussian filter and
  //  create one object of this type using the \code{New()} method.
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  typedef itk::DiscreteGaussianImageFilter< ImageType, ImageType >   
    GaussianFilterType;
  GaussianFilterType::Pointer   gaussianFilter =   GaussianFilterType::New();
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  The output of the SpatialObjectToImageFilter is connected as
  //  input to the DiscreteGaussianImageFilter.
  //
  //  Software Guide : EndLatex 
  
  //  Software Guide : BeginCodeSnippet
  gaussianFilter->SetInput(  imageFilter->GetOutput()  );
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  The variance of the filter is defined as a large value in order to
  //  increase the capture radius. Finally the execution of the filter is
  //  triggered using the \code{Update()} method.
  //
  //  Software Guide : EndLatex 

  //  Software Guide : BeginCodeSnippet
  const double variance = 20;
  gaussianFilter->SetVariance(variance);
  gaussianFilter->Update();
  //  Software Guide : EndCodeSnippet 


  //  Software Guide : BeginLatex
  //  
  //  Below we instantiate the type of the
  //  \doxygen{ImageToSpatialObjectRegistrationMethod} method and instantiate a
  //  registration object with the \code{New()} method. Note that the
  //  registration type is templated over the Image and the
  //  SpatialObject types. The spatial object in this case is the
  //  group of spatial objects.
  //
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!Instantiation}
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!New()}
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!Pointer}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef itk::ImageToSpatialObjectRegistrationMethod< ImageType, GroupType >
    RegistrationType;
  RegistrationType::Pointer registration = RegistrationType::New();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Now we instantiate the metric that is templated over
  //  the image type and the spatial object type. As usual, the \code{New()}
  //  method is used to create an object.
  //
  //  \index{itk::Image\-To\-Spatial\-Object\-Metric!Instantiation}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef SimpleImageToSpatialObjectMetric< ImageType, GroupType > MetricType;
  MetricType::Pointer metric = MetricType::New();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  An interpolator will be needed to evaluate the image at non-grid
  //  positions. Here we instantiate a linear interpolator type.
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef itk::LinearInterpolateImageFunction< ImageType, double >  
    InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The following lines instantiate the evolutionary optimizer.
  //
  //  \index{itk::One\-Plus\-One\-Evolutionary\-Optimizer!Instantiation}
  // 
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef itk::OnePlusOneEvolutionaryOptimizer  OptimizerType;
  OptimizerType::Pointer optimizer  = OptimizerType::New();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Next, we instantiate the transform class. In this case we use the
  //  Euler2DTransform that implements a rigid transform in $2D$
  //  space.
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef itk::Euler2DTransform<> TransformType;
  TransformType::Pointer transform = TransformType::New();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Evolutionary algorithms are based on testing random variations
  //  of parameters. In order to support the computation of random values,
  //  ITK provides a family of random number generators. In this example, we
  //  use the \doxygen{NormalVariateGenerator} which generates values with a
  //  normal distribution.
  //
  //  \index{itk::NormalVariateGenerator!New()}
  //  \index{itk::NormalVariateGenerator!Pointer}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  itk::Statistics::NormalVariateGenerator::Pointer generator 
    = itk::Statistics::NormalVariateGenerator::New();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The random number generator must be initialized with a seed.
  //
  //  \index{itk::NormalVariateGenerator!Initialize()}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  generator->Initialize(12345);
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The OnePlusOneEvolutionaryOptimizer is initialized by
  //  specifying the random number generator, the number of samples for the
  //  initial population and the maximum number of iterations.
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  optimizer->SetNormalVariateGenerator( generator );
  optimizer->Initialize( 10 );
  optimizer->SetMaximumIteration( 400 );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  As in previous registration examples, we take care to normalize the
  //  dynamic range of the different transform parameters. In particular, the
  //  we must compensate for the ranges of the angle and translations of the Euler2DTransform.
  //  In order to achieve this goal, we provide an array
  //  of scales to the optimizer. 
  //
  //  Software Guide : EndLatex 
 
  // Software Guide : BeginCodeSnippet
  TransformType::ParametersType parametersScale;
  parametersScale.set_size(3);
  parametersScale[0] = 1000; // angle scale

  for( unsigned int i=1; i<3; i++ )
    {
    parametersScale[i] = 2; // offset scale
    }
  optimizer->SetScales( parametersScale );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Here we instantiate the Command object that will act as an
  //  observer of the registration method and print out parameters at each
  //  iteration. Earlier, we defined this command as a class templated over the
  //  optimizer type. Once it is created with the \code{New()} method, we
  //  connect the optimizer to the command.
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  typedef IterationCallback< OptimizerType >   IterationCallbackType;
  IterationCallbackType::Pointer callback = IterationCallbackType::New();
  callback->SetOptimizer( optimizer );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  All the components are plugged into the
  //  ImageToSpatialObjectRegistrationMethod object. The typical
  //  \code{Set()} methods are used here. Note the use of the
  //  \code{SetMovingSpatialObject()} method for connecting the spatial object.
  //  We provide the blurred version of the original synthetic binary
  //  image as the input image.
  //
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!SetFixedImage()}
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!SetMovingSpatialObject()}
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!SetTransform()}
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!SetInterpolator()}
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!SetOptimizer()}
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!SetMetric()}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  registration->SetFixedImage( gaussianFilter->GetOutput() );
  registration->SetMovingSpatialObject( group );
  registration->SetTransform( transform );
  registration->SetInterpolator( interpolator );
  registration->SetOptimizer( optimizer );
  registration->SetMetric( metric );
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The initial set of transform parameters is passed to the registration
  //  method using the \code{SetInitialTransformParameters()} method. Note that
  //  since our original model is already registered with the synthetic image,
  //  we introduce an artificial mis-registration in order to initialize
  //  the optimization at some point away from the optimal value.
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  TransformType::ParametersType initialParameters( 
    transform->GetNumberOfParameters() );
  
  initialParameters[0] = 0.2;     // Angle
  initialParameters[1] = 7.0;     // Offset X
  initialParameters[2] = 6.0;     // Offset Y 
  registration->SetInitialTransformParameters(initialParameters);
  // Software Guide : EndCodeSnippet

  std::cout << "Initial Parameters  : " << initialParameters << std::endl;

  //  Software Guide : BeginLatex
  //
  //  Due to the character of the metric used to evaluate the fitness
  //  between the spatial object and the image, we must tell the optimizer that
  //  we are interested in finding the maximum value of the metric. Some
  //  metrics associate low numeric values with good matching, while others associate
  //  high numeric values with good matching. The \code{MaximizeOn()} and
  //  \code{MaximizeOff()} methods allow the user to deal with both types of
  //  metrics.
  //
  //  \index{itk::Optimizer!MaximizeOn()}
  //  \index{itk::Optimizer!MaximizeOff()}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  optimizer->MaximizeOn();
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  Finally, we trigger the execution of the registration process with the
  //  \code{StartRegistration()} method. We place this call in a
  //  \code{try/catch} block in case any exception is thrown during the
  //  process.
  //
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!StartRegistration()}
  //
  //  Software Guide : EndLatex 


  // Software Guide : BeginCodeSnippet
  try 
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & exp ) 
    {
    std::cerr << "Exception caught ! " << std::endl;
    std::cerr << exp << std::endl;
    }
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  //
  //  The set of transform parameters resulting from the registration can be
  //  recovered with the \code{GetLastTransformParameters()} method. This
  //  method returns the array of transform parameters that should be
  //  interpreted according to the implementation of each transform. In our
  //  current example, the Euler2DTransform has three parameters:
  //  the rotation angle, the translation in $x$ and the translation in $y$.
  //
  //  \index{itk::Image\-To\-Spatial\-Object\-Registration\-Method!StartRegistration()}
  //
  //  Software Guide : EndLatex 

  // Software Guide : BeginCodeSnippet
  RegistrationType::ParametersType finalParameters 
    = registration->GetLastTransformParameters();

  std::cout << "Final Solution is : " << finalParameters << std::endl;
  // Software Guide : EndCodeSnippet


  //  Software Guide : BeginLatex
  // 
  // \begin{figure}
  // \center
  // \includegraphics[height=0.44\textwidth]{ModelToImageRegistrationTraceAngle.eps}
  // \includegraphics[height=0.44\textwidth]{ModelToImageRegistrationTraceTranslations.eps}
  // \itkcaption[SpatialObject to Image Registration results]{Plots of the
  // angle and translation parameters for a registration process between an
  // spatial object and an image.}
  // \label{fig:ModelToImageRegistrationPlots}
  // \end{figure}
  //
  //  The results are presented in
  //  Figure~\ref{fig:ModelToImageRegistrationPlots}. The left side shows the
  //  evolution of the angle parameter as a function of iteration
  //  numbers, while the right side shows the $(x,y)$ translation.
  //
  //  Software Guide : EndLatex 

  return 0;
}
