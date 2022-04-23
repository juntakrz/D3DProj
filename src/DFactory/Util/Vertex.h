#pragma once

#include "../../pch.h"

namespace Vrtx
{
	class Layout
	{
	public:
		enum ElementType
		{
			Position2D = 0, Position3D, Texture2D, Normal, Tangent, Binormal, ColorFloat3, ColorFloat4
		};

		template<ElementType> struct Map;

		template<> struct Map<Position2D>
		{
			using Variable = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "POSITION";
		};

		template<> struct Map<Position3D>
		{
			using Variable = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "POSITION";
		};

		template<> struct Map<Texture2D>
		{
			using Variable = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* semantic = "TEXCOORD";
		};

		template<> struct Map<Normal>
		{
			using Variable = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "NORMAL";
		};

		template<> struct Map<Tangent>
		{
			using Variable = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "TANGENT";
		};

		template<> struct Map<Binormal>
		{
			using Variable = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "BINORMAL";
		};

		template<> struct Map<ColorFloat3>
		{
			using Variable = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* semantic = "COLOR";
		};

		template<> struct Map<ColorFloat4>
		{
			using Variable = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* semantic = "COLOR";
		};

		class Element
		{
		private:
			ElementType m_type;
			size_t m_offset;

			template<ElementType type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset)
			{
				return { Map<type>::semantic, 0, Map<type>::dxgiFormat, 0, (UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			}

		public:
			Element(ElementType type, size_t offset) : m_type(type), m_offset(offset) {};

			static constexpr size_t SizeOf(ElementType type) noexcept
			{
				switch (type)
				{
				case Position2D :
				{
					return sizeof(Map<Position2D>::Variable);
				}
				case Position3D:
				{
					return sizeof(Map<Position3D>::Variable);
				}
				case Texture2D:
				{
					return sizeof(Map<Texture2D>::Variable);
				}
				case Normal:
				{
					return sizeof(Map<Normal>::Variable);
				}
				case ColorFloat3:
				{
					return sizeof(Map<ColorFloat3>::Variable);
				}
				case ColorFloat4:
				{
					return sizeof(Map<ColorFloat4>::Variable);
				}
				case Tangent:
				{
					return sizeof(Map<Tangent>::Variable);
				}
				case Binormal:
				{
					return sizeof(Map<Binormal>::Variable);
				}
				}
			}

			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept
			{
				switch (m_type)
				{
				case Position2D:
				{
					return GenerateDesc<Position2D>(GetOffset());
				}
				case Position3D:
				{
					return GenerateDesc<Position3D>(GetOffset());
				}
				case Texture2D:
				{
					return GenerateDesc<Texture2D>(GetOffset());
				}
				case Normal:
				{
					return GenerateDesc<Normal>(GetOffset());
				}
				case ColorFloat3:
				{
					return GenerateDesc<ColorFloat3>(GetOffset());
				}
				case ColorFloat4:
				{
					return GenerateDesc<ColorFloat4>(GetOffset());
				}
				case Tangent:
				{
					return GenerateDesc<Tangent>(GetOffset());
				}
				case Binormal:
				{
					return GenerateDesc<Binormal>(GetOffset());
				}
				}
			}

			size_t Size() const noexcept
			{
				return SizeOf(m_type);
			}

			size_t GetOffset() const noexcept
			{
				return m_offset;
			}

			size_t GetOffsetAfter() const noexcept
			{
				return m_offset + Size();
			}

			ElementType GetType() const noexcept
			{
				return m_type;
			}
		};

	private:
		std::vector<Element> m_elements;

	public:
		size_t Size() const noexcept
		{
			return m_elements.empty() ? 0u : m_elements.back().GetOffsetAfter();
		}

		size_t GetElementCount() const noexcept
		{
			return m_elements.size();
		}

		template<ElementType type>
		const Element& Resolve() const noexcept
		{
			for (auto& it : m_elements)
			{
				if (it.GetType() == type)
				{
					return it;
				}
			}
			MessageBoxA(nullptr, "Incorrect element type.", "Vertex Template Error", MB_OK | MB_ICONWARNING);
			return m_elements.front();
		}

		const Element& ResolveByIndex(uint32_t index) const noexcept
		{
			return m_elements[index];
		}

		Layout& Append(ElementType type) noexcept
		{
			m_elements.emplace_back(type, Size());
			return *this;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> GetInputLayoutDesc() const noexcept
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> vecIED;

			vecIED.reserve(GetElementCount());
			for (const auto& it : m_elements)
			{
				vecIED.push_back(it.GetDesc());
			}

			return vecIED;
		}
	};

	class Vertex
	{
		friend class Buffer;

		char* m_pData = nullptr;
		const Layout& m_layout;

	private:
		template <Layout::ElementType DestLayoutType, typename SrcVar>
		void SetAttr(char* pAttribute, SrcVar&& value) noexcept
		{
			using DestVar = typename Layout::Map<DestLayoutType>::Variable;
			if constexpr (std::is_assignable<DestVar, SrcVar>::value)
			{
				*reinterpret_cast<DestVar*>(pAttribute) = value;
			}
			else
			{
				ASSERT(false);
			}
		}

		//recursively process and remove parameters until none left
		template <typename First, typename ...Rest>
		void SetAttrByIndex(size_t index, First&& first, Rest&&... rest)
		{
			SetAttrByIndex(index, std::forward<First>(first));
			SetAttrByIndex(index + 1, std::forward<Rest>(rest) ...);
		}

	protected:
		Vertex(char* pData, const Layout& layout) : m_pData(pData), m_layout(layout) {
			ASSERT(pData != nullptr);
		};

	public:
		template<Layout::ElementType type>
		auto& Attr() noexcept
		{
			auto pAttribute = m_pData + m_layout.Resolve<type>().GetOffset();
			return *reinterpret_cast<typename Layout::Map<type>::Variable*>(pAttribute);
		}

		template<typename T>
		void SetAttrByIndex(size_t index, T&& value) noexcept
		{
			const Layout::Element element = m_layout.ResolveByIndex(index);
			auto pAttribute = m_pData + element.GetOffset();

			switch (element.GetType())
			{
			case Layout::Position2D:
			{
				SetAttr<Layout::Position2D>(pAttribute, std::forward<T>(value));
				break;
			}
			case Layout::Position3D:
			{
				SetAttr<Layout::Position3D>(pAttribute, std::forward<T>(value));
				break;
			}
			case Layout::Texture2D:
			{
				SetAttr<Layout::Texture2D>(pAttribute, std::forward<T>(value));
				break;
			}
			case Layout::Normal:
			{
				SetAttr<Layout::Normal>(pAttribute, std::forward<T>(value));
				break;
			}
			case Layout::ColorFloat3:
			{
				SetAttr<Layout::ColorFloat3>(pAttribute, std::forward<T>(value));
				break;
			}
			case Layout::ColorFloat4:
			{
				SetAttr<Layout::ColorFloat4>(pAttribute, std::forward<T>(value));
				break;
			}
			case Layout::Tangent:
			{
				SetAttr<Layout::Tangent>(pAttribute, std::forward<T>(value));
				break;
			}
			case Layout::Binormal:
			{
				SetAttr<Layout::Binormal>(pAttribute, std::forward<T>(value));
				break;
			}
			default:
			{
				MessageBoxA(nullptr, "Incorrect vertex type.", "CVertex Error", MB_OK | MB_ICONWARNING);
				break;
			}
			}
		}
	};

	class ConstVertex
	{
		Vertex m_vertex;

	public:
		ConstVertex(const Vertex& v) : m_vertex(v) {};

		template<Layout::ElementType type>
		const auto& Attr() const noexcept
		{
			return const_cast<Vertex&>(m_vertex).Attr<type>();
		}
	};

	class Buffer
	{
	private:
		std::vector<char> m_vBuffer;
		Layout m_layout;

	public:
		Buffer(Layout layout) noexcept : m_layout(std::move(layout)) {};

		const char* GetData() const noexcept
		{
			return m_vBuffer.data();
		}

		const Layout& GetLayout() const noexcept
		{
			return m_layout;
		}

		size_t Size() const noexcept		// size in variables
		{
			return m_vBuffer.size() / m_layout.Size();
		}

		size_t SizeBytes() const noexcept
		{
			return m_vBuffer.size();
		}

		template<typename ...Params>
		void EmplaceBack(Params&&... params) noexcept
		{
			ASSERT(sizeof... (params) == m_layout.GetElementCount());
			m_vBuffer.resize(m_vBuffer.size() + m_layout.Size());
			Back().SetAttrByIndex(0u, std::forward<Params>(params)...);
		}

		Vertex Back() noexcept
		{
			ASSERT(m_vBuffer.size() != 0u);
			return Vertex{ m_vBuffer.data() + m_vBuffer.size() - m_layout.Size(), m_layout };
		}

		Vertex Front() noexcept
		{
			ASSERT(m_vBuffer.size() != 0u);
			return Vertex{ m_vBuffer.data(), m_layout };
		}

		Vertex operator[](size_t index) noexcept
		{
			ASSERT(index < Size());
			return Vertex(m_vBuffer.data() + m_layout.Size() * index, m_layout);
		}

		ConstVertex Back() const noexcept
		{
			return const_cast<Buffer*>(this)->Back();
		}

		ConstVertex Front() const noexcept
		{
			return const_cast<Buffer*>(this)->Front();
		}

		ConstVertex operator[](size_t index) const noexcept
		{
			return const_cast<Buffer&>(*this)[index];
		}
	};
}