<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

final class FileUpload {
    public function __construct(array $file, ?string $key=null);

    public function getName() : string {}

    public function getKey() : string {}

    public function getRealType() : string {}

    public function getSize() : string {}

    public function getTempName() : string {}

    public function getType() : string {}

    public function isUploadedFile() : bool {}

    public function getError() : string {}

    public function moveTo(string $destination) : bool {}
};
