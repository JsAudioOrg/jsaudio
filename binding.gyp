{
  'targets': [
  {
    'target_name': 'jsaudio',
    'sources': ['src/jsaudio.cc'],
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
                  '<(module_root_dir)/vendor/portaudio_x86.lib'
                ],
                'copies': [
                {
                  'destination': '<(module_root_dir)/build/Release/',
                  'files': [
                    '<(module_root_dir)/vendor/portaudio_x86.dll',
                    '<(module_root_dir)/vendor/portaudio_x86.lib',
                  ]
                }]
              }
            ],
            [
              'target_arch=="x64"', {
                "libraries": [
                  '<(module_root_dir)/vendor/portaudio_x64.lib'
                ],
                'copies': [
                {
                  'destination': '<(module_root_dir)/build/Release/',
                  'files': [
                    '<(module_root_dir)/vendor/portaudio_x64.dll',
                    '<(module_root_dir)/vendor/portaudio_x64.lib',
                  ]
                }]
              }
            ]
          ],
          "include_dirs": ["gyp/include"]
        }
      ]
    ]
  }]
}
