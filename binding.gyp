{
  'targets': [
  {
    'target_name': 'jsaudio',
    'sources': [
        'src/addon.cc',
        'src/jsaudio.cc',
        'src/helpers.cc',
        'src/stream.cc'
    ],
    'include_dirs': [
      '<!(node -e "require(\'nan\')")',
      '<(module_root_dir)/vendor/'
    ],
    "conditions": [
      [
        'OS=="win"', {
          "conditions": [
            [
              'target_arch=="ia32"', {
                "libraries": [
                  '<(module_root_dir)/vendor/win/portaudio_x86.lib'
                ],
                'copies': [
                {
                  'destination': '<(module_root_dir)/build/Release/',
                  'files': [
                    '<(module_root_dir)/vendor/win/portaudio_x86.dll',
                    '<(module_root_dir)/vendor/win/portaudio_x86.lib',
                  ]
                }]
              }
            ],
            [
              'target_arch=="x64"', {
                "libraries": [
                  '<(module_root_dir)/vendor/win/portaudio_x64.lib'
                ],
                'copies': [
                {
                  'destination': '<(module_root_dir)/build/Release/',
                  'files': [
                    '<(module_root_dir)/vendor/win/portaudio_x64.dll',
                    '<(module_root_dir)/vendor/win/portaudio_x64.lib',
                  ]
                }]
              }
            ]
          ],
          "include_dirs": ["gyp/include"]
        }
      ],
      [
        'OS=="mac"', {
          "libraries" : [
            '<(module_root_dir)/vendor/mac/libportaudio.a',
            '/Library/Frameworks/CoreAudio.framework',
            '/Library/Frameworks/AudioToolbox.framework',
            '/Library/Frameworks/AudioUnit.framework',
            '/Library/Frameworks/CoreServices.framework',
            '/Library/Frameworks/Carbon.framework'
          ],
          'cflags!': [ '-fno-exceptions' ],
          'cflags_cc!': [ '-fno-exceptions' ],
          'cflags_cc': [ '-std=c++0x' ]
        }
      ],
      [
        'OS=="linux"', {
          "libraries" : [
            '<(module_root_dir)/vendor/linux/libportaudio.so'
    	  ],
    	  'cflags!': [ '-fno-exceptions' ],
    	  'cflags_cc!': [ '-fno-exceptions' ],
    	  'cflags_cc': [ '-std=c++0x' ]
    	}
      ]
    ]
  }]
}
