# MeegooProto - C++ Zero-IDL Protobuf-like Serialization

## 项目状态
![GitHub](https://img.shields.io/github/stars/gqw/meegoo_pb?style=social)
<!-- ![CI/CD](https://github.com/gqw/meegoo_pb/actions/workflows/ci.yml/badge.svg) -->
![Version](https://img.shields.io/github/v/release/gqw/meegoo_pb)
![License](https://img.shields.io/badge/License-MIT-blue.svg)


## 项目描述
MeegooProto 是一个基于C++11/17模板元编程的序列化框架，通过编译期反射和自动类型推导技术，实现无需IDL文件即可实现高效Protobuf格式序列化和反序列化操作。它在保持C++原生类型系统的同时，提供接近手写序列化代码的性能表现，特别适合对性能敏感且需要灵活数据结构的现代C++项目。

## 核心特性
- ✅ **零IDL开发**：直接使用C++结构体/类进行序列化，无需编写额外描述文件
- ✅ **编译期优化**：利用constexpr反射生成序列化代码，消除运行时类型检查开销
- ✅ **高性能实现**：比Google Protobuf快xxx倍（根据测试用例）
- ✅ **零拷贝支持**：支持直接序列化到预分配内存或流对象
- ✅ **跨平台兼容**：支持Linux/macOS/Windows，已通过Clang/GCC/MSVC测试
- ✅ **强类型保障**：通过编译期约束确保序列化安全性，避免类型不匹配错误

<!--
## 性能对比

| 测试用例          | ProtoTonic | Protobuf 3.21 | 速度提升 | 内存占用 |
|-------------------|------------|---------------|----------|----------|
| 简单结构体        | 0.35μs     | 1.2μs         | 3.4x     | 减少38%  |
| 嵌套对象          | 0.89μs     | 3.1μs         | 3.5x     | 减少42%  |
| 大数据量数组      | 2.1μs      | 7.9μs         | 3.8x     | 减少45%  |

（测试环境：Intel i7-12700K, Ubuntu 22.04, Clang 15.0） -->


## 使用示例

```cpp
// 结构他体定义
struct SubTest2 {
    int32_t              i32      =   0;
    int64_t              i64      =   0;
    std::variant<float, std::string> variant;
};

struct TestAllData {
    int32_t              i32      =   0;
    int64_t              i64      =   0;
    uint32_t             u32      =   0;
    uint64_t             u64      =   0;
    meegoo::pb::sint32_t         si32     =   0;
    meegoo::pb::sint64_t         si64     =   0;
    meegoo::pb::fixed32_t        fx32     =   0;
    meegoo::pb::fixed64_t        fx64     =   0;
    meegoo::pb::sfixed32_t       sfx32    =   0;
    meegoo::pb::sfixed64_t       sfx64    =   0;
    bool                 bl       =   true;
    float                f32      =   0.0f;
    double               d64      =   0.0;
    std::string          str      =   "";
    meegoo::pb::bytes            bs       =   {};
    std::vector<int32_t> vec      =   {};
    std::map<int32_t, int32_t>  mp =  {};
    SubTest2             substruct =  {};
    std::variant<float, std::string> variant;
    meegoo::pb::google_any<int32_t>             any;
    int32_t              i32_2      =   0;
};
```

```cpp
int main() {
    TestAllData data;
    data.i32 = -100;
    data.i64 = -100;
    data.u32 = 100;
    data.u64 = 100;
    data.si32 = -100;
    data.si64 = -100;
    data.fx32 = 100;
    data.fx64 = 100;
    data.sfx32 = -100;
    data.sfx64 = -100;
    data.bl = true;
    data.f32 = 100.0f;
    data.d64 = 100.0;
    data.str = "100";
    data.bs = {1, 2, 3};
    data.vec = {3, 4, 5};
    data.mp = {{1, 2}, {3, 4}};
    data.substruct = {100, 100, 10.0f};
    data.variant = std::string("100");
    data.i32_2 = -200;
    data.any = {{"type.googleapis.com/pb.SubTest", 100}};

    std::string out;
    out.resize(pb_size(data));
    std::cout << "*** pb_size: " << out.size() << std::endl;
    auto ret = to_pb(data, out);
    std::cout << "size: " << out.size() << std::endl;
    for (size_t i = 0; i < out.size(); i++) {
        std::bitset<8> bits(out[i]);
        std::cout << std::hex  << bits << " ";
        if (i % 10 == 9) {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl;

    TestAllData data2;
    from_pb(data2, out);
    std::cout << "i32: " << data2.i32 << std::endl;
    std::cout << "si32: " << data2.si32 << std::endl;
    std::cout << "i32_2: " << data2.i32_2 << std::endl;
    return 0;
}
```

### 安装方法
```bash

# 构建
git clone https://github.com/gqw/meegoo_pb.git
cd meegoo_pb
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

```

### 文档与社区
- 📚 在线文档
- 💬 Discord 社区
- 📢 博客教程

### 贡献指南
欢迎提交 PR 改进性能或添加新功能，请遵循：
- 保持代码符合 C++17 标准
- 添加完整的单元测试
- 性能敏感代码需提供基准测试数据

### 许可证
MIT License - 允许商业使用、修改和分发，但需保留版权声明。

### 联系信息
- 📧 维护者邮箱：gqwmail@qq.com
- 🌐 项目官网：https://gqw.github.io

# 技术优势解析
1. 编译期反射：
    - 通过 std::tuple 和 std::variant 实现类型信息提取，无需运行时类型擦除
    - 通过 constexpr 实现编译器条件判定，避免运行时开销
2. 优化序列化策略：
    - 自动选择最优序列化格式（如变长整数、紧凑数组）
3. 内存管理：
    - 支持预分配内存缓冲区
3. 兼容性增强：
    - 支持嵌套的 STL 容器（vector/map/variant/any）