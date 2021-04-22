from setuptools import setup

setup(
      name='make-help-helper',
      version='0.1',
      description="Help menus generator for Makefiles",
      author='Oz N Tiram',
      author_email='oz.tiram@gmail.com',
      url='https://github.com/oz123/help-make-helper',
      license="MIT",
      zip_safe=False,
      keywords="make",
      classifiers=['Environment :: Console',
                   'Intended Audience :: End Users/Desktop',
                   'Intended Audience :: Developers',
                   'Intended Audience :: System Administrators',
                   'License :: OSI Approved :: BSD License',
                   'Operating System :: OS Independent',
                   'Programming Language :: Python',
                   'Programming Language :: Python :: 3',
                    ],

     scripts=['make-help-helper.py',]
)
