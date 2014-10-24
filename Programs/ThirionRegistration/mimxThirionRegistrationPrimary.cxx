#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include "itkImage.h"
#include "itkExceptionObject.h"
#include "itkBrains2MaskImageIOFactory.h"
#include "ThirionRegistration.h"
#include <metaCommand.h>


//This function prints the valid pixel types.
void PrintDataTypeStrings(void)
{
    //Prints the Input and output data type strings.
    std::cout << "UCHAR" << std::endl;
    std::cout << "SHORT" << std::endl;
    std::cout << "USHORT" << std::endl;
    std::cout << "INT" << std::endl;
    std::cout << "FLOAT" << std::endl;
}


//This function compares strings.
int CompareNoCase( const std::string &s, const std::string& s2 )
{
    //Compare strings.
    std::string::const_iterator p = s.begin();
    std::string::const_iterator p2 = s2.begin();

    while ( p != s.end() && p2 != s2.end() )
        {
        if ( toupper(*p) != toupper(*p2) ) return (toupper(*p) < toupper(*p2)) ? -1 : 1;
        p++;
        p2++;
        }

    return ( s2.size() == s.size() ) ? 0 : (s.size() < s2.size()) ? -1 : 1;


}


// This function calls the Thirion registration filter setting all the parameters.
    template < class InPixelType, class OutPixelType >
void ThirionFunction (MetaCommand command)
{
    const std::string InputFilename(command.GetValueAsString("InputFilename", "filename"));
    const std::string OutputFilename(command.GetValueAsString("OutputFilename", "filename"));
    const std::string TargetFilename(command.GetValueAsString("TargetFilename", "filename"));
    const std::string ParameterFilename(command.  GetValueAsString("ParameterFilename","filename"));
    const std::string OutputDeformationFilename(command.  GetValueAsString("OutputDeformationFieldname","filename"));
    const std::string CheckerboardFilename(command.  GetValueAsString("CheckerboardFilename","filename"));
    const std::string CheckerboardPattern(command.  GetValueAsString("CheckerPattern","PatternArray"));
    const std::string DisplacementPrefix(command.  GetValueAsString("DisplacementField","filename"));
    int MedianFilterRadius(command.GetValueAsInt("Median","radius"));
    std::cerr <<
      "InputFilename " << InputFilename << std::endl <<
      "OutputFilename " << OutputFilename << std::endl <<
      "TargetFilename " << TargetFilename << std::endl <<
      "ParameterFilename " << ParameterFilename << std::endl <<
      "OutputDeformationFilename " << OutputDeformationFilename << std::endl <<
      "CheckerboardFilename " << CheckerboardFilename << std::endl <<
      "CheckerboardPattern " << CheckerboardPattern << std::endl <<
      "DisplacementPrefix " << DisplacementPrefix << std::endl <<
      "MedianFilterRadius" << MedianFilterRadius << std::endl;

    const int dims =3;
    typedef itk::Image<InPixelType, dims> ImageType;
    typedef itk::Image<float, dims> RealImageType;
    typedef itk::Image<OutPixelType, dims> OutputImageType;
    typedef itk::Image< itk::Vector<float, 3>, 3> DeformationFieldType;

    typename DeformationFieldType::Pointer initialDeformationField;

    //
    // If optional initial transform is given, will use this transform to generate
    // a deformation field to prime the thirion demons registration.
    std::string initialTransformFilename(command.GetValueAsString("InitialTransform","filename"));
    
    //Need to explicity register the B2MaskIOFactory
    itk::Brains2MaskImageIOFactory::RegisterOneFactory ();

    //Thirion Registration application filter.
    typedef itk::ThirionRegistration < ImageType, RealImageType, OutputImageType > AppType;
    //Set the parameters for the filter.
   typename  AppType::Pointer app = AppType::New ();
   if(initialTransformFilename != "" )
     {
     app->SetInitialTransformFileName(initialTransformFilename);
      }
    app->SetParameterFileName (ParameterFilename.c_str ());
    app->SetTheMovingImageFileName (InputFilename.c_str ());
    app->SetTheFixedImageFileName (TargetFilename.c_str ());
    app->SetWarpedImageName (OutputFilename.c_str ());
    app->SetMedianFilterRadius(MedianFilterRadius);

    //Set the other optional arguments if specified by the user.
    if (DisplacementPrefix != "")
        {
        app->SetDisplacementBaseName (DisplacementPrefix.c_str ());
        }
    if (OutputDeformationFilename != "")
        {
        app->SetDeformationFieldOutputName (OutputDeformationFilename.c_str ());
        }

    if (CheckerboardFilename != "")
        {
        app->SetCheckerBoardFileName (CheckerboardFilename.c_str ());
        if (CheckerboardPattern != "")
            {
            unsigned int array[3] =
                { command.GetValueAsInt ("CheckerPattern", "XPattern"),
                command.GetValueAsInt ("CheckerPattern", "YPattern"),
                command.GetValueAsInt ("CheckerPattern", "ZPattern") };
            app->SetCheckerBoardPattern (array);
            }
        }

    if (command.GetValueAsBool ("Normalize", "norm"))
        {
        std::string normalize = "ON";
        app->SetOutNormalized (normalize.c_str ());
        }


    if (command.GetValueAsBool ("DEBUG", "debug"))
        {
        std::string debug = "ON";
        app->SetOutDebug (debug.c_str ());
        }

    //If Making BOBF option is specified Initialize its parameters
    if (command.GetValueAsBool ("BOBF", "bobf"))
        {
        if ((command.GetValueAsString ("BOBFTargetMaskname", "tgmask") == "")
            || (command.
                GetValueAsString ("BOBFTemplateMaskname", "tmpgmask") == ""))
            {
            std::cout <<
                "Error: If BOBF option is set then the target mask name and template mask file name should be specified. \n";
            exit(-1);
            }

        app->SetBOBFTargetMask (command.  GetValueAsString ("BOBFTargetMaskname", "tgmask").c_str ());
        app->SetBOBFTemplateMask (command.  GetValueAsString ("BOBFTemplateMaskname", "tmpgmask").c_str ());
        app->SetLower (command.GetValueAsInt ("BOBFLtshd", "ltshd"));
        app->SetUpper (command.GetValueAsInt ("BOBFUtshd", "utshd"));
       typename ImageType::SizeType radius;
        radius[0] = command.GetValueAsInt ("BOBFNeighX", "nbdx"); // Radius along X
        radius[1] = command.GetValueAsInt ("BOBFNeighY", "nbdy"); // Radius along Y
        radius[2] = command.GetValueAsInt ("BOBFNeighZ", "nbdz"); // Radius along Z
        app->SetRadius (radius);
        typename ImageType::IndexType seed;
        seed[0] = command.GetValueAsInt ("BOBFSeedX", "seedx"); // Seed in X dimension;
        seed[1] = command.GetValueAsInt ("BOBFSeedY", "seedy"); // Seed in Y dimension;
        seed[2] = command.GetValueAsInt ("BOBFSeedZ", "seedz"); // Seed in Z dimension;
        app->SetSeed (seed);
        }
        std::cout << "Setting Default PixelValue: " << command.GetValueAsInt ("BOBFBgnd", "bgnd") << "."<<std::endl;
        app->SetDefaultPixelValue (command.GetValueAsInt ("BOBFBgnd", "bgnd"));


    std::cout << "Running Thirion Registration" << std::endl;
    try
        {
        app->Execute ();
        }
    catch (itk::ExceptionObject & err)
        {
        std::cout << "Caught an ITK exception: " << std::endl;
        std::cout << err << " " << __FILE__ << " " << __LINE__ << std::endl;
        throw err;
        }
    catch (...)
        {
        std::
            cout << "Caught a non-ITK exception " << __FILE__ << " " << __LINE__
            << std::endl;
        }

    return ;
}

//This function processes the output data type.
    template < class PixelType > void
ProcessOutputType (MetaCommand command)
{
    const std::string OutType (command.
        GetValueAsString ("OutputPixelType",
            "PixelType"));
    if (command.GetValueAsString ("OutputPixelType", "PixelType") != "")
        {
        // process the string for the data type
        if (CompareNoCase (OutType.c_str (), std::string ("UCHAR")) == 0)
            {
            ThirionFunction < PixelType, unsigned char > (command);
            }
        else if (CompareNoCase (OutType.c_str (), std::string ("SHORT")) == 0)
            {
            ThirionFunction < PixelType, short > (command);
            }
        else if (CompareNoCase (OutType.c_str (), std::string ("USHORT")) == 0)
            {
            ThirionFunction < PixelType, unsigned short > (command);
            }
        else if (CompareNoCase (OutType.c_str (), std::string ("INT")) == 0)
            {
            ThirionFunction < PixelType, int > (command);
            }
        else if (CompareNoCase (OutType.c_str (), std::string ("FLOAT")) == 0)
            {
            ThirionFunction < PixelType, float > (command);
            }
        else
            {
            std::cout << "Error. Invalid data type for -outtype!  Use one of these:" << std::endl;
            PrintDataTypeStrings ();
            exit (-1);
            }

        }
    else
        {
        ThirionFunction < PixelType, float > (command);
        }
}


int ThrionRegistrationPrimary(int argc, char *argv[])
{

    MetaCommand command;

    //Moving image filename.
    command.SetOption("InputFilename","input",true,"InputFile name");
    command.AddOptionField("InputFilename","filename",MetaCommand::STRING,true);

    //Output image filename.
    command.SetOption("OutputFilename","output",true,"OutputFile name");
    command.AddOptionField("OutputFilename","filename",MetaCommand::STRING,true);

    //Target image filename.
    command.SetOption("TargetFilename","target",true,"TargetFile name");
    command.AddOptionField("TargetFilename","filename",MetaCommand::STRING,true);

    //Parameter filename containing histogram parameters and number of resolution levels.
    command.SetOption("ParameterFilename","p",true,"ParameterFile name");
    command.AddOptionField("ParameterFilename","filename",MetaCommand::STRING,true);

    command.SetOption("InputPixelType","intype",true,"InputPixel Type UCHAR|SHORT|USHORT|INT|FLOAT");
    command.AddOptionField("InputPixelType","PixelType",MetaCommand::STRING,true);

    //The images will be written in this type. The default is input pixel type.
    command.SetOption("OutputPixelType","outtype",false,"OutputPixel Type UCHAR|SHORT|USHORT|INT|FLOAT");
    command.AddOptionField("OutputPixelType","PixelType",MetaCommand::STRING,false);

    //The prefix of the displacementfield to be written. X,Y,Z displacement fields will be written with the prefix added to them.
    command.SetOption("DisplacementField","dispfields",false,"DisplacementField Prefix");
    command.AddOptionField("DisplacementField","Prefix",MetaCommand::STRING,false);

    //Checkerboard option gives the checker image of the fixed image and the output image.
    command.SetOption("CheckerboardFilename","checkerbd",false,"CheckerFile name");
    command.AddOptionField("CheckerboardFilename","filename",MetaCommand::STRING,false);

    //Checker patterns for the checker board image.
    command.SetOption("CheckerPattern","cbpattern",false,"CheckerBoard Pattern");
    command.AddOptionField("CheckerPattern","XPattern",MetaCommand::INT,false,"4");
    command.AddOptionField("CheckerPattern","YPattern",MetaCommand::INT,false,"4");
    command.AddOptionField("CheckerPattern","ZPattern",MetaCommand::INT,false,"4");

    //Writes the deformation field to the filename specified by this option.
    command.SetOption("OutputDeformationFieldname","defwrite",false,"Deformation Field Output.");
    command.AddOptionField("OutputDeformationFieldname","filename",MetaCommand::STRING,false);

    //This option allows to warp and write the normalized images to output. In normalized images the image values are shfit-scaled to be between 0 and 1
    command.SetOption("Normalize","norm",false,"Warp Normalized Images.");
    command.AddOptionField("Normalize","norm",MetaCommand::FLAG,false);

    //This Option helps you to write the images after each step
    command.SetOption("DEBUG","debug",false,"Write intermediate Images.");
    command.AddOptionField("DEBUG","debug",MetaCommand::FLAG,false);


    //Make BOBF  images. 
    command.SetOption("BOBF","bobf",false,"Make BOBF Images (use -debug flag to view results)");
    command.AddOptionField("BOBF","bobf",MetaCommand::FLAG,false);

    //BOBF Fixed Mask filename.
    command.SetOption("BOBFTargetMaskname","tgmask",false,"Target Mask name to perform BOBF");
    command.AddOptionField("BOBFTargetMaskname","tgmask",MetaCommand::STRING, false);

    //BOBF Moving Mask filename.
    command.SetOption("BOBFTemplateMaskname","tmpmask",false,"Template Mask name to perform BOBF");
    command.AddOptionField("BOBFTemplateMaskname","tmpgmask",MetaCommand::STRING, false);

    //Lower Threshold for the BOBF
    command.SetOption("BOBFLtshd","ltshd",false,"Lower Threshold for performing BOBF. Default 0");
    command.AddOptionField("BOBFLtshd","ltshd",MetaCommand::INT,false,"0");

    //Backgrnd Replace Value for the BOBF
    command.SetOption("BOBFBgnd","bgnd",false,"Background fill with this value Default 0");
    command.AddOptionField("BOBFBgnd","bgnd",MetaCommand::INT,false,"0");

    //Upper Threshold for the BOBF
    command.SetOption("BOBFUtshd","utshd",false,"Upper Threshold for performing BOBF. Default 70");
    command.AddOptionField("BOBFUtshd","utshd",MetaCommand::INT,false,"70");

    //Seed X for BOBF
    command.SetOption("BOBFSeedX","seedx",false,"Seed X for BOBF. Default 0");
    command.AddOptionField("BOBFSeedX","seedx",MetaCommand::INT,false,"0");

    //Seed Y for BOBF
    command.SetOption("BOBFSeedY","seedy",false,"Seed Y for BOBF. Default 0");
    command.AddOptionField("BOBFSeedY","seedy",MetaCommand::INT,false,"0");

    //Seed Z for BOBF
    command.SetOption("BOBFSeedZ","seedz",false,"Seed Z for BOBF. Default 0");
    command.AddOptionField("BOBFSeedZ","seedz",MetaCommand::INT,false,"0");

    //X Neighborhood to be included for BOBF
    command.SetOption("BOBFNeighX","nbdx",false,"X Neighborhood to be included for BOBF. Default 1");
    command.AddOptionField("BOBFNeighX","nbdx",MetaCommand::INT,false,"1");

    //Y Neighborhood to be included for BOBF
    command.SetOption("BOBFNeighY","nbdy",false,"Y Neighborhood to be included for BOBF. Default 1");
    command.AddOptionField("BOBFNeighY","nbdy",MetaCommand::INT,false,"1");

    //Z Neighborhood to be included for BOBF
    command.SetOption("BOBFNeighZ","nbdz",false,"Z Neighborhood to be included for BOBF. Default 1");
    command.AddOptionField("BOBFNeighZ","nbdz",MetaCommand::INT,false,"1");

    command.SetOption("Median","median",false,"Apply median filter to input images");
    command.AddOptionField("Median","radius",MetaCommand::INT,false,"0");

    command.SetOption("InitialTransform","InitialTransform",false,"Initial Transform for registration");
    command.AddOptionField("InitialTransform","filename",MetaCommand::STRING,false,"");
    


    if (!command.Parse(argc,argv))
        {
        return 1;
        }

    std::cout << "Running as: \n";
    for(int i=0; i<argc; i++)
        {
        std::cout << " " << argv[i];
        }
    std::cout << std::endl;

    //Test if the input data type is valid
    const std::string PixelType(command.GetValueAsString("InputPixelType","PixelType"));

    if ( command.GetValueAsString("InputPixelType","PixelType") != "")
        {
        // check to see if valid type
        if (( CompareNoCase( PixelType.c_str(), std::string("UCHAR" ) ) ) &&
            ( CompareNoCase( PixelType.c_str(), std::string("SHORT" ) ) ) &&
            ( CompareNoCase( PixelType.c_str(), std::string("USHORT") ) ) &&
            ( CompareNoCase( PixelType.c_str(), std::string("INT"   ) ) ) &&
            ( CompareNoCase( PixelType.c_str(), std::string("FLOAT" ) ) )
        )
            {
            std::cout << "Error. Invalid data type string specified with -intype!" << std::endl;
            std::cout << "Use one of the following:" << std::endl;
            PrintDataTypeStrings();
            exit(-1);
            }
        }

    const std::string OutPixelType(command.GetValueAsString("OutputPixelType",
            "PixelType" ));

    if ( command.GetValueAsString("OutputPixelType","PixelType" ) != "")
        {
        // check to see if valid type
        if( ( CompareNoCase( OutPixelType.c_str(), std::string("UCHAR" ) ) ) &&            ( CompareNoCase( OutPixelType.c_str(), std::string("SHORT") ) ) &&
            ( CompareNoCase( OutPixelType.c_str(), std::string("USHORT") ) ) &&
            ( CompareNoCase( OutPixelType.c_str(), std::string("INT"   ) ) ) &&
            ( CompareNoCase( OutPixelType.c_str(), std::string("FLOAT" ) ) )
           )

            {
            std::cout << "Error. Invalid data type string specified with -intype!" << std::endl;
            std::cout << "Use one of the following:" << std::endl;
            PrintDataTypeStrings();
            exit(-1);
            }
        }


    //Call the process output data type function based on the input data type.
    const std::string InType(command.GetValueAsString("InputPixelType",
            "PixelType"));

    if (CompareNoCase (InType, std::string ("UCHAR")) == 0)
        {
        ProcessOutputType < unsigned char > (command);
        }
    else if (CompareNoCase (InType, std::string ("SHORT")) == 0)
        {
        ProcessOutputType < short > (command);
        }
    else if (CompareNoCase (InType, std::string ("USHORT")) == 0)
        {
        ProcessOutputType < unsigned short > (command);
        }
    else if (CompareNoCase (InType, std::string ("INT")) == 0)
        {
        ProcessOutputType < int > (command);
        }
    else if (CompareNoCase (InType, std::string ("FLOAT")) == 0)
        {
        ProcessOutputType < float > (command);
        }
    else
        {
        std::cout << "Error. Invalid data type for -intype!  Use one of these:" << std::endl;
        PrintDataTypeStrings ();
        exit (-1);
        }




    return 0;
}

