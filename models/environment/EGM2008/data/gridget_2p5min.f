      program gridget
c      
c-----------------------------------------------------------------------
c
c     GRIDGET IS A F77 PROGRAM THAT READS AN INPUT FILE OF GLOBALLY
c     GRIDDED POINT VALUES, EXTRACTS ALL GRID VALUES WITHTIN A USER- 
c     DEFINED SUBREGION, AND OUTPUTS THESE GRID VALUES TO AN ASCII FILE.
c
c     THE INPUT FILE OF GLOBALLY GRIDDED POINT VALUES WILL STORE THESE  
c     VALUES IN REAL*4 SEQUENTIAL BINARY FORMAT, WITH ONE RECORD FOR 
c     EACH PARALLEL BAND. THESE INPUT VALUES ARE EQUALLY SPACED IN 
c     LATITUDE AND LONGITUDE, AND ARE SITUATED AT THE CORNERS OF THEIR 
c     RESPECTIVE CELLS, SUCH THAT THE TOP-LEFT POINT IN THE GRID HAS A 
c     LONGITUDE OF ZERO DEGREES AND A LATITUDE OF NINETY DEGREES. THE 
c     FIRST RECORD CONTAINS THE NORTHERN-MOST PARALLEL, AND THE FIRST 
c     VALUE IN EACH RECORD IS THE WESTERNMOST VALUE FOR THAT PARALLEL. 
c     NOTE THAT GRID VALUES SITUATED ON THE ZERO MERIDIAN APPEAR ONLY
c     ONCE AS THE FIRST VALUE IN THEIR RESPECTIVE RECORD, AT ZERO 
c     LONGITUDE. THESE VALUES ARE NOT REPEATED AT THE END OF THEIR 
c     RESPECTIVE RECORDS, AT LONGITUDE = 360 DEGREES. 
c
c     GRIDGET PROVIDES FOR TWO MODES OF ASCII-FORMATTED OUTPUT OF THE 
c     EXTRACTED GRID VALUES.
c
c     IN THE FIRST OUTPUT MODE, THE FIRST LINE OF THE ASCII OUTPUT FILE 
c     CONTAINS HEADER INFORMATION, WHICH SPECIFIES FOR THE OUTPUT GRID:
c
C     1) LATITUDE OF THE TOP LEFT POINT IN DEGREES,
C     2) LONGITUDE OF THE TOP LEFT POINT IN DEGREES,
C     3) LATITUDINAL GRID-SPACING IN MINUTES,
C     4) LONGITUDINAL GRID-SPACING IN MINUTES,
C     5) NUMBER OF ROWS IN THE OUTPUT GRID,
C     6) NUMBER OF COLUMNS IN THE OUTPUT GRID.
c
c     THE REMAINING LINES IN THE ASCII FILE CONTAIN ALL THE GRIDDED
c     VALUES WITHIN THE USER-DEFINED SUBREGION, WITH ONE RECORD PER
c     OUTPUT PARALLEL BAND. THE FIRST OF THESE RECORDS CONTAINS ALL 
c     VALUES FOR THE NORTHERN-MOST PARALLEL, AND THE FIRST VALUE IN 
c     EACH RECORD IS THE WESTERNMOST VALUE FOR THAT PARALLEL.    
c
c     IN THE SECOND OUTPUT MODE, THE ASCII OUTPUT FILE CONTAINING THE  
c     EXTRACTED GRID VALUES CONTAINS ONE RECORD FOR EACH GRID POINT. 
c     EACH RECORD CONTAINS THE GEODETIC LATITUDE AND LONGITUDE IN 
c     DECIMAL DEGREES, FOLLOWED BY THE EXTRACTED VALUE FOR THAT 
c     GRIDPOINT. THE POINTS ARE LISTED IN THEIR SCANLINE SEQUENCE, SUCH 
c     THAT THE NORTHWESTERN-MOST (TOP LEFT) GRID POINT IS LISTED FIRST 
c     AND THE SOUTH-EASTERN (BOTTOM RIGHT) GRID POINT IS LISTED LAST.
c
c     THE INPUT PARAMETERS FOR GRIDGET ARE:
c
c     iout     :  SELECTS OUTPUT MODE. SET iout=1 FOR OUTPUT IN AN ASCII
c                 GRID, WHERE EACH RECORD CONTAINS ALL THE EXTRACTED
c                 GRIDPOINTS FOR A GIVEN PARALLEL. SET iout=2 FOR THE 
c                 ASCII OUTPUT TO CONTAIN ONE RECORD FOR EACH EXTRACTED
c                 GRID POINT.
c
c     path_in  :  PATH TO THE REAL*4 SEQUENTIAL BINARY INPUT FILE, 
c     name_in  :  NAME OF THE REAL*4 SEQUENTIAL BINARY INPUT FILE ,
c     path_out :  PATH TO THE ASCII OUTPUT FILE,
c     name_out :  NAME OF THE ASCII OUTPUT FILE,
c
c     dnorth   :  NORTHERN BOUNDARY OF SUBREGION IN DEGREES,
c     dsouth   :  SOUTHERN BOUNDARY OF SUBREGION IN DEGREES,
c     deast    :  EASTERN  BOUNDARY OF SUBREGION IN DEGREES,
c     dwest    :  WESTERN  BOUNDARY OF SUBREGION IN DEGREES,
c
c     dlat     :  LAT SPACING (DEGREES) OF INPUT GRID,
c     dlon     :  LON SPACING (DEGREES) OF INPUT GRID.
c
c     FOR SUBREGIONS WHICH STRIDE THE GREENWICH MERIDIAN, USE A NEGATIVE
c     VALUE FOR dwest AND A POSITIVE VALUE FOR deast.
c      
c-----------------------------------------------------------------------
c     ORIGINAL PROGRAM:                           SIMON HOLMES, JUL 2007
c     MODIFIED FOR CORNER-CELL REGISTRATION       SIMON HOLMES, MAY 2008
c     MUTLI-OUTPUT OPTION                         SIMON HOLMES, MAY 2008 
c-----------------------------------------------------------------------
      implicit real*8(a-h,o-z)
      character*120 path_in,p_in,name_in,n_in,fnu1
      character*120 path_out,pout,name_out,nout,fnu10
c-----------------------------------------------------------------------
c
c     INPUT PARAMETERS
c
c-----------------------------------------------------------------------
      parameter(
     &   path_in  = './',
     &   name_in  = 'Und_min2.5x2.5_egm2008_isw=82_WGS84_TideFree_SE',
     &   path_out = './',
     &   name_out = 'OUTPUT.DAT') 
c-----------------------------------------------------------------------
      parameter(dlat     = 2.5d0/60.d0,
     &          dlon     = 2.5d0/60.d0)   
c-----------------------------------------------------------------------
c
c     NON-INPUT PARAMETERS
c
c----------------------------------------------------------------------- 
      parameter(nrows    = 4321,
     &          ncols    = 8640)
c-----------------------------------------------------------------------       
      real*8 scrd(ncols,2),statd(22),flat(nrows),flon(ncols)
      real*4 grid(ncols),temp(ncols)
c-----------------------------------------------------------------------
      write(6,5000)
      write(6,400)
  400 format(30x,'Execution')
      write(6,5000)
c-----------------------------------------------------------------------
c
c     PROMPT FOR GEOGRAPHIC REGION
c
c-----------------------------------------------------------------------
  350 print *, ' '
      print *, 'Enter limits(degrees): north south west east'
	print *, '(longitudes 0-360, unless crossing prime meridian)'
	read (5, *) dnorth, dsouth, dwest, deast

	if (dnorth .gt. 90.d0 .or. dsouth .lt. -90.d0 .or.
     &    dwest .gt. deast .or. dsouth .gt. dnorth .or.
     &    deast .lt. 0.d0) then
	  write (6, '(a)') 'INVALID REGION'
	  goto 350 
	end if
c-----------------------------------------------------------------------
c
c     PROMPT FOR OUTPUT FORMAT
c
c-----------------------------------------------------------------------
  360 print *, ' '
	print *, 'Output mode? (grid = 1,lat/lon/geoid height = 2)'
	read (5, *) iout

	if (.not. (iout .eq. 1 .or. iout .eq. 2)) then
	  write (6, '(a)') 'INVALID, enter 1 or 2'
	  goto 360 
	end if
c-----------------------------------------------------------------------
c
c     OPEN INPUT AND OUTPUT FILES
c
c-----------------------------------------------------------------------
      call extract_name_120(path_in,p_in,nchr_pin)
      call extract_name_120(name_in,n_in,nchr_nin)
      fnu1=p_in(1:nchr_pin)//n_in(1:nchr_nin)
c
	open(1,file=fnu1,form='unformatted',
     .     status='old',iostat=ios)
c
      call extract_name_120(path_out,pout,nchr_pot)
      call extract_name_120(name_out,nout,nchr_not)
      fnu10=pout(1:nchr_pot)//nout(1:nchr_not)
c
      open(10,file=fnu10,form='formatted',iostat=ios)
c
      write(6,410) n_in
  410 format(10x,'Input Sequential Binary Data File : ',a120,//)
c
      write(6,420) nout
  420 format(10x,'Output Extracted Ascii Data File : ',a120,//)
      if (iout.eq.1) then
        write(6,430)
      elseif (iout.eq.2) then
        write(6,440)
      else
        write(6,450)
        write(6,5000)
        stop
      write(6,5000)
      endif  ! iout
  430 format(10x,'ONE OUTPUT RECORD PER EXTRACTED PARALLEL BAND')
  440 format(10x,'ONE OUTPUT RECORD PER EXTRACTED GRIDPOINT')
  450 format(10x,'ERROR: IOUT MUST BE SET TO 1 OR 2')  
      write(6,5000)
c-----------------------------------------------------------------------
c
c     INITIALISE DIMENSIONS OF EXTRACTED GRID
c
c-----------------------------------------------------------------------
      iglob  = 1-(int(((deast-dwest)/360.d0) + 1.d-8))
c
      in   = nint(((90.d0-dnorth)/dlat)-1.d-8)+1
      is   = nint(((90.d0-dsouth)/dlat)+1.d-8)+1
      jw   = nint((dwest/dlon)-1.d-8)+1
      je   = nint((deast/dlon)+1.d-8)+iglob
      irow = is-in+1
      jcol = je-jw+1
c
      jw1 = jw
      if (jw1.lt.1    ) jw1 = jw1+ncols
      if (jw1.gt.ncols) jw1 = jw1-ncols 
c
      je1 = je
      if (je1.lt.1    ) je1 = je1+ncols
      if (je1.gt.ncols) je1 = je1-ncols    
c
      tlat = 90.d0 - (in -1)*dlat
      wlon =         (jw1-1)*dlon
      slat = 90.d0 - (is -1)*dlat
      elon =         (je1-1)*dlon
c
      write(6,500) tlat,slat,wlon,elon,dlat*60.d0,dlon*60.d0,irow,jcol
c
  500 format(10x,'Geometry of Extracted Grid:',//,
     &10x,'Latitude of northern-most points = ',f10.5,' (Degrees).',/,
     &10x,'Latitude of southern-most points = ',f10.5,' (Degrees).',/,
     &10x,'Longitude of western-most points = ',f10.5,' (Degrees).',/,
     &10x,'Longitude of eastern-most points = ',f10.5,' (Degrees).',/,
     &10x,'                Latitude spacing = ',f10.5,' (Minutes).',/,
     &10x,'               Longitude spacing = ',f10.5,' (Minutes).',/,//
     &10x,i6,' Rows x ',i6,' Columns of values output.')
c
      call flush(6)
c
      dn = 90.d0 -(in-1)*dlat  !  for stats sub only
      dw =        (jw-1)*dlon  
c-----------------------------------------------------------------------
c
c     DIMENSIONS OK?
c
c-----------------------------------------------------------------------
      if (dnorth.gt.90.d0.or.dnorth.lt.-90.d0.or.
     &    dsouth.gt.90.d0.or.dsouth.lt.-90.d0.or.  
     &    dsouth.gt.dnorth.or.
     &    dwest.gt.deast.or.
     &    deast.lt.0.d0.or.
     &    jcol.gt.ncols) then
         write(6,5000)
         write(6,550) 
         write(6,5000)
         stop
      endif  !  dim         
 550  format(10x,'ERROR: CHECK INPUT DIMENSIONS')
c-----------------------------------------------------------------------
c
c     EXTRACT GRID ONE ROW AT A TIME; COMPUTE STATS FOR EXTRACTED GRID
c
c-----------------------------------------------------------------------
      if (iout.eq.1) then
        write(10,600) tlat,wlon,dlat*60.d0,dlon*60.d0,irow,jcol
      endif  !  iout
 600  format(2(f12.6,1x),2(f8.3,1x),2(i6,1x)) 
c-----------------------------------------------------------------------
      if (iout.eq.2) then
        do i = 1, nrows
          flat(i) = 90.d0 -(i-1)*dlat
        enddo  !  i
      endif  !  iout
c-----------------------------------------------------------------------
      do i = 1, in-1
        read(1)
      enddo  !  i
c
      write(6,5000)
      write(6,700)
  700 format(10x,'Statistics of Extracted Values',//)
c
      ii = 0
      do i = in, is
c
        read(1) (grid(j), j=1,ncols)
        ii = ii + 1
        jj = 0
        do j = jw, je
          j1 = j
          if (j1.lt.1    ) j1 = j1+ncols
          if (j1.gt.ncols) j1 = j1-ncols 
          jj = jj+1
          temp(jj)   = grid(j1)
          scrd(jj,1) = grid(j1)
          scrd(jj,2) = 1.d0
          flon(jj)   = (j1-1)*dlon
        enddo  !  j    
c
        if (iout.eq.1) write(10,800) (temp(j), j=1,jj)
        if (iout.eq.2) then
          do j = 1, jj
            write(10,810) flat(i),flon(j),temp(j)
          enddo  !  j
        endif  !  iout       
  800   format(10f9.3)
  810   format(2(1x,f11.6),2x,f9.3)
        call stats(dn,dw,ii,dlat,dlon,irow,jcol,scrd,9999.d0,0,statd,0)    
c
      enddo  !  i  
      close(10)
c-----------------------------------------------------------------------
c
c     READ OUTPUT AND PRINT SAMPLE: TOP LEFT 10 PT X 10 PT SQUARE
c
c-----------------------------------------------------------------------
      if (iout.eq.1) then
        write(6,5000)
        write(6,750)
  750   format(10x,'Sample Output: Top Left 10pt x 10pt Square',//)
c
        open(10,file=fnu10,form='formatted',status='old',iostat=ios)
        read(10,  *) tlin,wlin,dlatm,dlonm,nrowi,ncolj
        write(6,600) tlin,wlin,dlatm,dlonm,nrowi,ncolj
c
        if (nrowi.gt.10) nrowi = 10
        if (ncolj.gt.10) ncolj = 10
c
        do i = 1, nrowi 
          read(10,  *) (temp(j), j=1,ncolj)
          write(6,800) (temp(j), j=1,ncolj)
        enddo  !  i  
        close(10)
      endif  !  iout
c-----------------------------------------------------------------------
      write(6,5000)
      write(6,1000)
      write(6,5000)
c
 1000 format(27x,'Normal Termination')
c           
 5000 format(//,'c',71('-'),//)
c
      stop
      end
c-----------------------------------------------------------------------
c
      SUBROUTINE STATS(TOPLAT,WSTLON,I,GRDN,GRDE,is,NCOLS,DATA,
     $                 EXCLUD,ISIG,STAT,icent)
C-----------------------------------------------------------------------
      IMPLICIT REAL*8(A-H,O-Z)
      CHARACTER*20 DLABEL(14)
      CHARACTER*20 SLABEL( 8)
      DIMENSION DATA(NCOLS,2)
      DOUBLE PRECISION DEXCLUD,DG,SD,STAT(22)
      SAVE
      data ix/0/
      DATA PI/3.14159265358979323846D+00/
      DATA DLABEL/'    Number of Values','  Percentage of Area',
     $            '       Minimum Value',' Latitude of Minimum',
     $            'Longitude of Minimum','       Maximum Value',
     $            ' Latitude of Maximum','Longitude of Maximum',
     $            '     Arithmetic Mean','  Area-Weighted Mean',
     $            '      Arithmetic RMS','   Area-Weighted RMS',
     $            '   Arithmetic S.Dev.','Area-Weighted S.Dev.'/
      DATA SLABEL/'       Minimum Sigma',' Latitude of Minimum',
     $            'Longitude of Minimum','       Maximum Sigma',
     $            ' Latitude of Maximum','Longitude of Maximum',
     $            'Arithmetic RMS Sigma','Area-wghtd RMS Sigma'/
C-----------------------------------------------------------------------
      IF(I.EQ.1) THEN
      DEXCLUD=EXCLUD
      DTR=PI/180.D0
      FOURPI=4.D0*PI
      DPR=GRDN*DTR
      DLR=GRDE*DTR
      CAREA=2.D0*DLR*SIN(DPR/2.D0)
      DO 10 K=1,22
      STAT(K)=0.D0
   10 CONTINUE
      STAT( 3)= DEXCLUD
      STAT( 6)=-DEXCLUD
      STAT(15)= DEXCLUD
      STAT(18)= 0.D0
      ENDIF
C-----------------------------------------------------------------------
c
      dlat = toplat -(i-1.d0)*grdn - (grdn/2.d0)*icent
      COLATC=(90.D0-DLAT)*DTR
      AREA=CAREA*SIN(COLATC)
c
c   The following statement avoids over-estimating the total area
c   covered by the grid (of POINT values) when the pole(s) are part
c   of the grid.
c
      if(abs(dlat).eq.90.d0) area=2.d0*dlr*sin(dpr/4.d0)**2
c
C-----------------------------------------------------------------------
      DO 110 J=1,NCOLS
      DLON=WSTLON+(J-1.D0)*GRDE +(GRDE/2.D0)*icent
      DG=DATA(J,1)
      SD=DATA(J,2)
      IF(DG.LT.DEXCLUD) THEN
C-----------------------------------------------------------------------
      STAT( 1)=STAT( 1)+1.D0
      STAT( 2)=STAT( 2)+AREA
      IF(DG.LE.STAT( 3)) THEN
      STAT( 3)=DG
      STAT( 4)=DLAT
      STAT( 5)=DLON
      ENDIF
      IF(DG.GE.STAT( 6)) THEN
      STAT( 6)=DG
      STAT( 7)=DLAT
      STAT( 8)=DLON
      ENDIF
      STAT( 9)=STAT( 9)+DG
      STAT(10)=STAT(10)+DG*AREA
      STAT(11)=STAT(11)+DG**2
      STAT(12)=STAT(12)+DG**2*AREA
      IF(SD.LE.STAT(15)) THEN
      STAT(15)=SD
      STAT(16)=DLAT
      STAT(17)=DLON
      ENDIF
      IF(SD.GE.STAT(18)) THEN
      STAT(18)=SD
      STAT(19)=DLAT
      STAT(20)=DLON
      ENDIF
      STAT(21)=STAT(21)+SD**2
      STAT(22)=STAT(22)+SD**2*AREA
C-----------------------------------------------------------------------
      ENDIF
  110 CONTINUE
C-----------------------------------------------------------------------
      IF(I.NE.is) RETURN
      IF(STAT(1).GT.0.D0) THEN
      STAT( 9)=STAT( 9)/STAT( 1)
      STAT(10)=STAT(10)/STAT( 2)
      STAT(11)=SQRT(STAT(11)/STAT( 1))
      STAT(12)=SQRT(STAT(12)/STAT( 2))
c-----------------------------------------------------------------------
      STAT(13)=0.d0
      STAT(14)=0.d0
      temp1 = STAT(11)**2-STAT( 9)**2
      temp2 = STAT(12)**2-STAT(10)**2
      if (temp1.gt.0.d0) STAT(13)=SQRT(temp1)
      if (temp2.gt.0.d0) STAT(14)=SQRT(temp2)
c-----------------------------------------------------------------------
c     STAT(13)=SQRT(STAT(11)**2-STAT( 9)**2)
c     STAT(14)=SQRT(STAT(12)**2-STAT(10)**2)
c-----------------------------------------------------------------------
      STAT(21)=SQRT(STAT(21)/STAT( 1))
      STAT(22)=SQRT(STAT(22)/STAT( 2))
      STAT( 2)=STAT( 2)/FOURPI*100.D0
      ELSE
      DO 120 J=3,22
      STAT(J)=DEXCLUD
  120 CONTINUE
      ENDIF
C=======================================================================
      NUM=INT(STAT(1))
      WRITE(6,6001) DLABEL(1),NUM
 6001 FORMAT(5X,A20,3X,I11)
      DO 210 K=2,14
      WRITE(6,6002) DLABEL(K),STAT(K)
 6002 FORMAT(5X,A20,3X,F16.4)
  210 CONTINUE
      WRITE(6,6003)
 6003 FORMAT(' ')
      IF(ISIG.EQ.1) THEN
      DO 220 K=1,8
      WRITE(6,6002) SLABEL(K),STAT(K+14)
  220 CONTINUE
      ENDIF
C=======================================================================
      RETURN
      END
c-----------------------------------------------------------------------
      subroutine extract_name_120(old_name,name,n)
      implicit none
      integer*4 i,n
      character old_name*120,name*120,char*1
c
      n = 0
      do i = 1, 120
        char = old_name(i:i)
        if (char .ne. ' ') then
          n = n + 1
          name(n:n) = char
        endif  !  char
      enddo  !  i
c
      return
      end
c-----------------------------------------------------------------------
