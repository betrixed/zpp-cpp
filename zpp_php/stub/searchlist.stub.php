<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

interface IfFindLeaf {
    public function findLeaf(string $leaf, ?array $extensions = null) : null|string {}

    public function getPaths() : ?array {}

    public function addPaths(array $paths) : void {}

    public function addPath(string $path) : void {}

    public function setPaths(array $paths) : void {}

    public function clear() : void {}
};

class SearchList implements IfFindLeaf {
    public function __construct(?array $paths = null);

    public function findLeaf(string $leaf, ?array $extensions = null) : null|string {}

    public function getPaths() : ?array {}

    public function addPaths(array $paths) : void {}

    public function clear() : void {}

    public function addPath(string $path) : void {}

    public function hasPath(string $path) : bool {}

    public function setPaths(array $paths) : void {}
};