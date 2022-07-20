#include <iostream>

#include <arrow/api.h>
#include <arrow/dataset/api.h>
#include <arrow/compute/api.h>
#include <arrow/filesystem/api.h>
#include <parquet/file_reader.h>
#include <arrow/testing/builder.h>
 
#include "arrow/memory_pool.h"
#include "arrow/util/config.h"
#include "arrow/datum.h"
#include "arrow/util/byte_size.h"
#include "arrow/type_traits.h"

#include <algorithm>
namespace arrow {

  using ArrayVector = std::vector<std::shared_ptr<Array>>;

  int func()
  { 
      static int i;
      return ++i % 2;
  }


template <typename TYPE, typename C_TYPE = typename TYPE::c_type>
void ArrayFromVector(const std::shared_ptr<DataType>& type,
                     const std::vector<bool>& is_valid, const std::vector<C_TYPE>& values,
                     std::shared_ptr<Array>* out, MemoryPool* pool = default_memory_pool()) {
  auto type_id = TYPE::type_id;
  ASSERT_EQ(type_id, type->id())
      << "template parameter and concrete DataType instance don't agree";

  std::unique_ptr<ArrayBuilder> builder_ptr;
  std::cerr << "MakeArrayBuilder\n";
  ASSERT_OK(MakeBuilder(pool, type, &builder_ptr));
  // Get the concrete builder class to access its Append() specializations
  auto& builder = dynamic_cast<typename TypeTraits<TYPE>::BuilderType&>(*builder_ptr);

  for (size_t i = 0; i < values.size(); ++i) {
    if (is_valid[i]) {
      ASSERT_OK(builder.Append(values[i]));
    } else {
      ASSERT_OK(builder.AppendNull());
    }
    //std::cerr << "  Append\n";

  }
  ASSERT_OK(builder.Finish(out));
  std::cerr << "Finish\n";

}

  void test() {

    auto pool = MemoryPool::CreateDefault();
    LoggingMemoryPool logging_pool(pool.get());

    ArrayVector arrays_one_;
    ArrayVector arrays_another_;

    std::shared_ptr<ChunkedArray> one_;
    std::shared_ptr<ChunkedArray> another_;

    std::vector<bool> null_bitmap(100);
    std::vector<int32_t> data(100);

    std::iota(data.begin(), data.end(), 0);
    std::generate(null_bitmap.begin(), null_bitmap.end(), func);

    std::shared_ptr<Array> array;
    auto type = TypeTraits<Int32Type>::type_singleton();
    arrow::ArrayFromVector<Int32Type, int32_t>(type, null_bitmap, data, &array, &logging_pool);
    arrays_one_.push_back(array);
    arrays_another_.push_back(array);

    one_ = std::make_shared<ChunkedArray>(arrays_one_);
    if (!arrays_another_.empty()) {
      another_ = std::make_shared<ChunkedArray>(arrays_another_);
    }
    std::cerr << "pool : " << pool->bytes_allocated() << std::endl;

   auto one_size = util::TotalBufferSize(*one_);
   auto another_size =    util::TotalBufferSize(*another_);
   std::cerr << "one_size : " << one_size << std::endl;
   std::cerr << "another_size : " << another_size << std::endl;  
  }
}
int main(int argc, char* argv[]) {
  
  arrow::test();
  return 0;
}
