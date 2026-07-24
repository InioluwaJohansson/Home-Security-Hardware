#include "JWTAuth.h"

void JwtManager::SetToken(const String& token)
{
    _token = token;
}

const String& JwtManager::GetToken() const
{
    return _token;
}

bool JwtManager::HasToken() const
{
    return !_token.isEmpty();
}

void JwtManager::Clear()
{
    _token.clear();
}