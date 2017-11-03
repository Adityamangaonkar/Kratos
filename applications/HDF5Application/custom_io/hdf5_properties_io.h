//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main author:    Michael Andre, https://github.com/msandre
//

#if !defined(KRATOS_HDF5_PROPERTIES_IO_H_INCLUDED)
#define KRATOS_HDF5_PROPERTIES_IO_H_INCLUDED

// System includes
#include <string>

// External includes

// Project includes
#include "includes/define.h"
#include "includes/io.h"

// Application includes
#include "hdf5_application_define.h"
#include "custom_io/hdf5_file.h"

namespace Kratos
{
namespace HDF5
{
///@addtogroup HDF5Application
///@{

/// A class for properties IO of a model part in HDF5.
class PropertiesIO : public IO
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition
    KRATOS_CLASS_POINTER_DEFINITION(PropertiesIO);

    ///@}
    ///@name Life Cycle
    ///@{

    /// Constructor.
    PropertiesIO(std::string Prefix, File::Pointer pFile);

    ///@}
    ///@name Operations
    ///@{
    void ReadProperties(PropertiesContainerType& rThisProperties) override;

    void WriteProperties(Properties const& rThisProperties) override;

    void WriteProperties(PropertiesContainerType const& rThisProperties) override;
    ///@}

protected:
    ///@name Protected Operations
    ///@{

    ///@}

private:
    ///@name Member Variables
    ///@{
    std::string mPrefix;
    File::Pointer mpFile;
    ///@}

};

///@} addtogroup
} // namespace HDF5.
} // namespace Kratos.

#endif // KRATOS_HDF5_PROPERTIES_IO_H_INCLUDED defined
