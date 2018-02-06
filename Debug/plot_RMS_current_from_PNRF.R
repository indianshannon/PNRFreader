plot_RMS_current_from_PNRF <- function(filename){
  
  exe="PNRFreader.exe"
  if(file.exists(exe))
  {
    command = paste(exe,filename,sep=" ");
    ########### Pass the PNRF file to be parsed by C++ compiled binary##############
        # Output will be stored in a file suffixed with "xxxxx-Channel-3.txt" (.pnrf extension stripped. See the below two statements) 
                    system(command,invisible=FALSE);
    ################################################################################
    
    filename_without_ext <- sub(".pnrf$","",filename,fixed=FALSE);
    filename_out <- paste(filename_without_ext,"-Channel-3.txt",sep="");  #The output filename where data is stored.
  
    ######### PLOTTING ##############
    library(ggplot2);
    
    time_current <- as.matrix(read.table(filename_out,header=TRUE,sep="\t",as.is=TRUE));
    sampleslimit <- 20000; #20k samples to be plot
    start <- 2.8*1e6;  #some random starting point
    print(sprintf("nrows=%d, ncols=%d, but we limit to %d rows",nrow(time_current),ncol(time_current),sampleslimit));
    
    time_current1 <- time_current[start:(start+sampleslimit-1),]
    time_current2 <- matrix(0,sampleslimit,2);
    iv <- seq(1,sampleslimit);
    
    # RMS average over a SLIDING WINDOW of 10ms = 0.01s
    it2<- 1;
    for (it1 in iv) {
      if(it2<sampleslimit){
        it2<-it2+1;
      }
      while(time_current1[it2,1]<time_current1[it1,1]+0.01)
      {
        if(it2<sampleslimit)
        {
          it2<-it2+1;
        }
        else
        {
          break;
        }
      }
      
      if(it2>it1)
      {
        it2<-it2-1;
      }
      
      time_current2[it1,1]<-time_current1[it1,1];
      time_current2[it1,2]<-sqrt(mean(time_current1[it1:it2,2]^2));
    }
    
    # Prepare a frame for use with ggplot2
    mydata <- data.frame(time = time_current1[,1], current = time_current1[,2], rmscurrent = time_current2[,2]);
    
    #ggplot2 for plotting multiple lines on the same graph.
    ggplot(data=mydata,mapping=aes(x=time,y=current))+
      geom_line(aes(y=current,color="blue"))+
      geom_line(aes(y=rmscurrent,color="red"))+
      scale_color_manual(labels=c("Measured","RMS,10ms window"),values=c("blue","red"))+
      labs(color="Direct Current")
    
  }
  else
  {
  print("PNRFreader.exe not found. It should be in the working directory:");
  getwd();
  }
}