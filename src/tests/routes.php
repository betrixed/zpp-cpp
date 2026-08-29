<?php
use Wcc\RouteSet;
/* A random comment */
return [
    "user" => [
        RouteSet::FUNX_S => "<verb>",
        "@key" => [
            '@user' => ['user','Join\UserController']
        ],
        "noprefix" => true,
        "routes" => [
            'GET /reset-password/:code/:email' => '@user->resetPwd',
            'GET /confirm-email/:code/:email' => '@user->confirmEmail',
        ]
    ],
    "stats" => [
        "@key" => [
            '@stats' => "Wc\\LinkClick",
            ],
        RouteSet::FUNX_S => "<verb>",
        "not_found" => ["controller" => 'error', "action" => 'route404'],
        "noprefix" => true,
        RouteSet::MOD_S => "default",
        RouteSet::HITS_S => false,
        "roles" => ['Guest'],
        "routes" => [
            'PUT /statput [ajax]' => '@stats->link',
        ],
    ],
    "app" => [
        "@key" => [
            '@index' => "App\\IndexView",
            '@events' => ['blog', "Blog\\EventList"],
            '@email' =>  ['contact',  "Contact\\Form"],
            '@article' => ['blog', "Blog\\Article"],
        ],
        RouteSet::FUNX_S => "<verb>",
        "not_found" => ["controller" => 'error', "action" => 'route404'],
        "noprefix" => true,
        RouteSet::MOD_S => "default",
        RouteSet::HITS_S => true,
        "roles" => ['Guest'],
        "routes" => [
            'GET /links' => '@index->links',
            'GET /' => '@index->index',
            'GET /index' => '@index->index',
            'GET /index.php' => '@index->index',
            'GET /pcan.php' => '@index->index',
            'GET /events' => '@events->index',
            
            'POST /contact-us [ajax]' => '@email->submit',
            'GET /contact-us [ajax?]' => '@email->email',
            'GET /article/:title' => '@article->title',
        ]
    ]
];
