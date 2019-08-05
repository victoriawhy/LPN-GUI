c     Created by Mahdi Esmaily Moghadam 05-25-2011
c     Please report any problem to mesmaily@ucsd.edu, memt63@yahoo.com

      MODULE COM

      LOGICAL pCorr, qCorr

      INTEGER nUnknowns, nDirichletSrfs, nNeumannSrfs, nXprint,
     2 nptsElvmax
 

      REAL(8) pConv, qConv

      INTEGER, ALLOCATABLE :: srfToXPtr(:), srfToXdPtr(:)
      
      REAL(8), ALLOCATABLE :: QNeumann(:,:), PDirichlet(:,:), offset(:),
     2   Xprint(:)
      END MODULE COM
