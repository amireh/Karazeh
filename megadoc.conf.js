var path = require('path');

module.exports = {
  assetRoot: path.resolve(__dirname),
  outputDir: path.resolve(__dirname, 'doc/compiled'),

  title: 'Karazeh',

  layoutOptions: {
    banner: false,
    rewrite: {
      '/readme.html': '/index.html',
    },

    customLayouts: [
      {
        match: { by: 'namespace', on: [ 'articles' ] },
        regions: [
          {
            name: 'Layout::Sidebar',
            outlets: [
              {
                name: 'Markdown::Browser',
                options: { flat: true },
              }
            ]
          },

          {
            name: 'Layout::Content',
            options: { framed: true },
            outlets: [
              {
                name: 'Markdown::Document',
              },
            ],
          },

          {
            name: 'Layout::NavBar',
            options: { framed: true },
            outlets: [
              {
                name: 'Markdown::DocumentTOC'
              }
            ]
          }
        ]
      }
    ]
  },

  plugins: [
    require('megadoc-theme-qt')(),

    require('megadoc-plugin-markdown')({
      baseURL: '/',
      sanitize: false,
      source: [
        'README.md',
        'doc/**/*.md'
      ]
    })
  ]
};