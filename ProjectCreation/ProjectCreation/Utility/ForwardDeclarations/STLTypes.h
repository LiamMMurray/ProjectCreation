#pragma once
namespace std
{
        template <class _Kty>
        struct hash;

        template <class _Ty = void>
        struct equal_to;

        template <class _Ty>
        class allocator;

        template <class _Ty1, class _Ty2>
        struct pair;

        template <class _Kty,
                  class _Ty,
                  class _Hasher = hash<_Kty>,
                  class _Keyeq  = equal_to<_Kty>,
                  class _Alloc  = allocator<pair<const _Kty, _Ty>>>
        class unordered_map;
} // namespace std