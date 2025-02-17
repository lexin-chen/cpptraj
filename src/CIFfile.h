#ifndef INC_CIFFILE_H
#define INC_CIFFILE_H
#include "Atom.h"
#include "BufferedLine.h"
#include <map>
/// Used to access CIF files
class CIFfile {
  private:
    typedef std::vector<std::string> Sarray;

  public:
    /// ----- Used to hold CIF data blocks -------
    class DataBlock {
      public:
        DataBlock() {}
        std::string const& Header() const { return dataHeader_;         }
        bool empty()                const { return dataHeader_.empty(); }
        int AddHeader(std::string const&);
        int AddSerialDataRecord(const char*, BufferedLine&);
        int AddLoopColumn(const char*, BufferedLine&);
        int AddLoopData(const char*, BufferedLine&);
        void Append(DataBlock const&);
        void ListData() const;
        int ColumnIndex(std::string const&) const;
        /// \return Serial data for given ID
        std::string Data(std::string const&) const;
        // Iterators
        typedef std::vector<Sarray>::const_iterator data_it;
        data_it begin() const { return columnData_.begin(); }
        data_it end()   const { return columnData_.end();   }
      private:
        static int ParseData(std::string const&, std::string&, std::string&);
        int GetColumnData(int, BufferedLine&, bool);

        std::string dataHeader_; ///< The data header, e.g. '_atom_site'
        Sarray columnHeaders_;   ///< Column headers, e.g. 'label_atom_id'
        std::vector<Sarray> columnData_; ///< Array of column data, e.g.:
          /*
    ATOM 1    N N    . SER A 1 1  ? -2.559 9.064   0.084   1.00 0.00 ? ? ? ? ? ? 1  SER A N    1
    ATOM 2    C CA   . SER A 1 1  ? -3.245 8.118   0.982   1.00 0.00 ? ? ? ? ? ? 1  SER A CA   1
           */
    };
    /// ----- Used to hold CIF data --------------
    class CIFdata {
      public:
        CIFdata(std::string const&);
        /// Add data block to CIF data
        int AddDataBlock(DataBlock const&);
        /// Get data block with specified header
        DataBlock const& GetDataBlock(std::string const&) const;
      private:
        typedef std::map<std::string, DataBlock> CIF_DataType;
        /// Map block names to DataBlocks
        CIF_DataType cifdata_;
        /// data_ name
        std::string dataName_;
    };
    // -------------------------------------------

    CIFfile() {}
    static bool ID_CIF( CpptrajFile& );
    int Read(FileName const&,int);
    /// \return const reference to the specified data block.
    FileName const& CIFname() const { return file_.Filename(); }
    /// Get data from most recently added data set
    DataBlock const& GetDataBlock(std::string const&h) const { return data_.back().GetDataBlock(h); }

    DataBlock const& GetBlockWithColValue(std::string const&, std::string const&,
                                          std::string const&) const;

  private:
    //int AddDataBlock(DataBlock const&);

    enum mode { UNKNOWN = 0, SERIAL, LOOP };
    BufferedLine file_;
    //typedef std::map<std::string, DataBlock> CIF_DataType;
    //CIF_DataType cifdata_;
    static const DataBlock emptyblock;
    /// Hold all CIF data
    std::vector<CIFdata> data_;
};

#endif
