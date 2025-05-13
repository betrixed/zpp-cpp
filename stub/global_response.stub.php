<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

interface IfCipher
{
    public function decrypt(string $input, ?string $key = null) : string{}

    public function decryptBase64(string $input, ?string $key = null) : string{}

    public function encrypt(string $input, ?string $key = null) : string{}

    public function encryptBase64(string $input, ?string $key = null) : string{}

    public function getAvailableCiphers() : array{}

    public function getAuthTag() : string{}

    public function getAuthData() : string{}

    public function getAuthTagLength() :  int{}

    public function getCipher() : string{}

    public function getKey() : string{}

    public function setAuthTag(string $tag) : IfCipher{}

    public function setAuthData(string $data) : IfCipher{}

    public function setAuthTagLength(int $len) : IfCipher{}

    public function setCipher(string $cipher) :  IfCipher{}

    public function setKey(string $key) :  IfCipher{}

    public function setPadding(int $scheme) :  IfCipher{}

    public function useSigning(bool $useSigning) :  IfCipher{}
};

interface IfEvent
{
    public function getData() : mixed {}

    public function getSource(): object {}

    public function getType() : mixed {}

    public function isCancelable(): bool {}

    public function isStopped(): bool {}

    public function setData(mixed $data ): IfEvent;

    public function setType(string $type): IfEvent;

    public function stop(): IfEvent;
};

interface IfFilter {
    public function sanitize(
        array|string $value,
        array|string $names,
        bool $noRecursive=false
    ): mixed {}
};

interface IfEventQueue
{
    public const int DEFAULT_PRIORITY = 100;

    public function attach(
        string $eventType, 
        mixed  $handler,
        int    $priority = IfEventQueue::DEFAULT_PRIORITY
    ): void {}

    public function detach(string $eventType, mixed $handler): void {}

    public function detachAll(?string $type = null): void {}

    public function fire(
        string $eventType,   object $source,
        mixed  $data = null,   bool $cancelable = true
    ): mixed {}

    public function getListeners(string $type): array {}

    public function hasListeners(string $type): bool {}
};

/*
class Headers {
    public function get(string $name) : string | bool {}
    public function has(string $name) : bool {}
    public function remove(string $header) : Headers {}
    public function reset() : Headers {}
    public function send() : bool {}
    public function set(string $name, string $value)  : Headers {}
    public function setRaw(string $header) : Headers {}
    public function toArray() : array {}
};
*/



class Response {
    public function __construct(
            ?string $content = null,  ?int $code = null, ?string $status = null);

    public function ajaxHtml(string $content): void {}

    public function ajaxJson(array $data): void {}

    public function appendContent(string $content): void {}

    public function delay_redirect(string $location, int $delay=2): void {}

    public function fireEvent(string $eventType): mixed {}

    public function getEventQueue(): ?EventQueue {}

    public function getContent(): string {}

    public function getHeaders():  Hmap {}

    public function getStatusCode(): ?int {}

    public function hasContent() : bool {}

    public function hasHeader(string $name): bool {}

    public function isSent(): bool {}

    public function redirect(
        ?string $location = null, 
        bool $externalRedirect = false, 
        int $statusCode = 302
    ) : Response {}

    public function resetHeaders(): void {}

    public function send(): bool {}

    public function sendCookies(): bool {}

    public function sendHeaders(): bool {}

    public function setContent(string $content): void {}

    public function setContentLength(int $contentLength): void {}

    public function setContentType(
        string $contentType, ?string $charset = null): void {}

    public function setCookies(CookieBag $bag): void {}

    public function setExpires(DateTime $datetime): void {}

    public function setFileToSend(
        string $filePath, 
        ?string $attachmentName = null, 
        bool $attachment = true): void {}

    public function setHeader(string $name, string $value): void {}

    public function setHeaders(Headers $hdr): void {}

    public function setJsonContent( 
        string $content, 
        int $jsonOptions = 0, 
        int $depth = 512): void {}

    public function setNotModified(): void {}

    public function setRawHeader(string $header): void {}

    public function setStatusCode(
        int $code, ?string $message = null): void {}
};

/*
class Cookie {
    public function __construct(
        string $name,
        mixed $value = null,
        int $expire = 0,
        ?string $path = "/",
        ?bool $secure = null,
        ?string $domain = null,
        ?bool $httpOnly = null,
        array $options = []
    );

    public function __toString() : string {}
    public function delete() : void {}
    public function getDomain() : string {}
    public function getExpiration() : int {}
    public function getHttpOnly() : bool {}
    public function getName() : string{}
    public function getOptions() : array{}
    public function getPath() : string{}
    public function getSecure() : bool{}
    public function getValue(
        string|array|null $filters, mixed $default = null
    ): mixed{}

    public function isEncrypted() : bool {}
    public function restore() : Cookie {}
    public function setDomain(string $domain) : Cookie {}
    public function setExpiration(int $expire) : Cookie {}
    public function setHttpOnly(?bool $httpOnly)  : Cookie {}
    public function setOptions(array $options)  : Cookie {}
    public function setPath(string $path) : Cookie {}
    public function setSecure(?bool $secure) : Cookie {}
    public function setSignKey(?string $signKey = null) : Cookie {}
    public function setValue(mixed $value) : Cookie {}
    public function useEncryption(bool $useEncryption) : Cookie {}
};

class CookieBag {
    public function __construct(bool $useEncryption = false, ?string $signKey = null);

    public function delete(string $name) :  bool {}

    public function get(string $name) : Cookie {}

    public function getCookies(): array {}

    public function has(string $name) : bool {}

    public function isSent() : bool {}

    public function isUsingEncryption() : bool {}

    public function reset() : CookieBag {}

    public function send() : bool {}

    public function set(
        string $name,
        mixed $value = null,
        int $expire = 0,
        ?string $path = "/",
        ?bool $secure = null,
        ?string $domain = null,
        ?bool $httpOnly = null,
        array $options = []
    ) : CookieBag {}

    public function setSignKey(?string $signKey = null) : CookieBag {}
    public function useEncryption(bool $useEncryption) : CookieBag {}
};
*/