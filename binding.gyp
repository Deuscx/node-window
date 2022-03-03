{
  "targets": [
    {
      "target_name": "wm",
      "sources": [ "src/main.cpp"],
      "cflags": ["-fexceptions -std=c++11"],
      "cflags_cc": ["-fexceptions"],
      'conditions': [
         ['OS=="win"', {
           'sources': ["src/main.cpp", "src/windows/window.cpp"]
            }
         ],
         ['OS=="mac"', {
           'sources': ["src/main.cpp", "src/macos/window.cpp"],
           'xcode_settings': {
             'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
           }
         }
         ],
      ],
      'include_dirs': [
           "<!@(node -p \"require('node-addon-api').include\")"
       ],
      'dependencies': [
         "<!(node -p \"require('node-addon-api').gyp\")"
       ],
       'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    },
  ]
}
