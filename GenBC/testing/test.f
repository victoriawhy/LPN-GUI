! This test program is a test bed for GenBC. It replaces a three-dimensional 
! model with a cheap "surrogate". In this case, we take the tutorial case of the
! cylinder with the prescribed inflow and RCR outflow. We replace this cylinder
! with a single resistor and solve for the Pressure at the Dirichlet inflow
! and solve for Flow at the Neumann outflow. This might seem counter-intuitive,
! but remember that this needs to produce the opposite quantities as the type
! of boundary. Since GenBC applies a flow boundary condition on the Dirichlet
! surfaces, this test.f needs to produce pressure at those surfaces to feed
! to GenBC. Using the same logic, test.f needs to produce flow for all the
! Neumann surfaces.
!  
! To run this test case, first you need to compile it with:
!  
!  gfortran test.f
!  
! Then, run the test program with:
!  
! ./a.out
!  
! Or compile with:
!
!  make
!
! And run with:
!
! ./test
      
      PROGRAM ODE_TEST

      IMPLICIT NONE
 
!     Declare all the variables.

      INTEGER nFaces, nTimeSteps,nDFaces,nNFaces
      PARAMETER (nFaces=2, nTimeSteps=2, nDFaces=1,nNFaces=1)
      REAL*8 Q(nFaces), P(nFaces), R(nFaces-1), dt, Qtemp, Ptemp
      CHARACTER S
      INTEGER i, j, n, nCycles
 

      CALL system('rm -f QGeneral')
      CALL system('rm -f PGeneral')
      CALL system('rm -f InitialData')
      CALL system('rm -f AllData')
      
      OPEN (2, FILE='QGeneral', STATUS='UNKNOWN', ACCESS='APPEND')

!     This is the cylinder resistance
      R=1.334D3*(/1.0/)

!     This sets the timestep size
      dt = 1D-2

!     These values are initialized by GenBC, but not yet by GenBC_cy
      P = 0D0 !1.0D2
      Q = 0D0 !-1D0

      nCycles = 1

      Do n=1, nCycles
         DO i=1, nTimeSteps
            OPEN (1,FILE='GenBC.int',STATUS='UNKNOWN',
     2      FORM='UNFORMATTED')
            WRITE (1) 'L'
            WRITE (1) dt
            WRITE (1) nDFaces
            WRITE (1) nNFaces
            
!           Q(1) is the flow at the Neumann outlet. Since its a single tube,
!           the flow here is the same as the flow at the inlet (Q(2)) but with
!           opposite sign
            Q(1) = -Q(2)
            
!           This is the pressure at the Dirichlet inlet. It takes the pressure
!           at the Neumann outlet and adds the pressure drop across the resistor
            P(1) = P(2) + Q(1)*R(1)
            
            
!           The code below communicates with GenBC
            WRITE(1) P(1), P(1)
            
            WRITE(1) Q(1), Q(1)
            
            PRINT *, "P(1) ", P(1)
            PRINT *, "Q(1) ", Q(1)

            CLOSE(1)
            
            CALL system('./GenBC_cy') ! Change this to switch between GenBC and GenBC_cy
            OPEN (1,FILE='GenBC.int',STATUS='OLD',FORM='UNFORMATTED')

            READ (1) Q(2)
            READ (1) P(2)

            PRINT *, "Q(2) ", Q(2)
            PRINT *, "P(2) ", P(2)

            CLOSE(1)


         END DO
      END DO
      CLOSE(2)

      STOP
      END PROGRAM ODE_TEST
