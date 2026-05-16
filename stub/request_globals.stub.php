<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class RequestGlobals  
{
    public function __construct();

    public function getHeader(string $header): mixed {}

    public function setHeader(string $header, mixed $value): void {}

    public function hasHeader(string $header): bool {}

    public function getBasicAuth(): ?array {}
    
    public function getServerName(): string {}
    
    public function getMethod(): int {}

    public function isAjax(): bool {}

    public function getScheme(): string {}

    public function getURI(bool $onlyPath = false): string {}
    
    public function getAcceptableContent(): array {}

    public function getHeaders(): array {}

    public function hasRequest(string $name): bool {}

    public function isMethod(mixed $methods, bool $strict = false): bool {}
    
    public function isOptions(): bool {}

    public function isPatch(): bool {}

    public function isPost(): bool {}

    public function isPut(): bool {}

    public function isPurge(): bool {}

    public function isSecure(): bool {}

    public function numFiles(bool $onlySuccessful = false): int {}

    public function hasFiles(): bool {}

    public function getBestAccept(): string {}

    public function getClientAddress(bool $trustForwardedHeader = false): ?string {}

    public function hasQuery(string $name): bool {}

    public function getJsonRawBody(bool $associative = false) : array | object | false {}

    public function getRawBody(): string {}

    public function getLanguages(): array {}

    public function getUserAgent(): string {}

    public function getBestCharset(): string {}

    public function getClientCharsets(): array {}

    public function getBestLanguage(): string {}

    public function getPort(): int {}

    public function getContentType(): ?string {}

    public function getHttpHost(): string {}

    public function getUploadedFiles(bool $onlySuccessful = false, bool $namedKeys = false): array {}

    public function getDigestAuth(): array {}

    public function getHTTPReferer(): string {}

    public function getServerAddress(): string {}

    public function isSoap(): bool {}

    public function getMethodOverride(): bool {}

    public function setMethodOverride(bool $v) : void {}

    public function getSpoofMethod(): bool {}

    public function setSpoofMethod(bool $val) : void {}

    public function setStrictHost(bool $val) : void {}

    public function setAuthCallback(mixed $callback) : void {}

    public function post() : Hmap {}

    public function query() : Hmap {}

    public function server() : Hmap {}

    public function getPost() : array {}

    public function getQuery() : array {}

    public function getServer() : array {}
   
};
